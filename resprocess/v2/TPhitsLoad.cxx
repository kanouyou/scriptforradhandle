#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <sys/stat.h>
#include <TTree.h>
#include <TFile.h>
#include "TPhitsLoad.h"


TPhitsLoad :: TPhitsLoad()
    : fR(new double[2]),
      fMesh(0),
      fRmin(0.),
      fRmax(99.),
      fTh(1.)
{
  fR[0] =  99999.;
  fR[1] = -99999.;
}

TPhitsLoad :: TPhitsLoad(const char* filename)
    : fR(new double[2]),
      fMesh(0),
      fRmin(0.),
      fRmax(99.),
      fTh(1.)
{
  fR[0] =  99999.;
  fR[1] = -99999.;

  Load(filename);
}

TPhitsLoad :: ~TPhitsLoad()
{
  delete [] fR;
}

void TPhitsLoad :: Load(const char* filename)
{
#if defined(DEBUG)
  std::cout << " -- loading file: " << filename << std::endl;
#endif

  TFile* file = new TFile(filename);
  if (!file) {
    std::cerr << "Error: cannot open the file: " << filename << std::endl;
    throw;
  }

  if (fOrigData.size()>0) {
    std::cerr << "Error: the array of data has been filled." << std::endl;
    throw;
  }

  int tot_tree = file->GetNkeys();
#if defined(DEBUG)
  std::cout << " -- checking the total trees in this tree file: " << tot_tree << std::endl;
#endif

  for (int i=0; i<tot_tree; i++)
    EventLoop(file, i+1);

  FindMesh();
#if defined(DEBUG)
  std::cout << " -- finished loading" << std::endl;
#endif
}

void TPhitsLoad :: FindMesh()
{
  // calculate mesh
  const int totalmesh = fOrigData.size() / fPosz.size();
  fMesh = sqrt(totalmesh);
  
  std::cout << " -- total mesh: " << totalmesh << ", mesh size: " << fMesh << "x" << fMesh << std::endl;

  for (auto i=0; i<fOrigData.size(); i++) {
    if (fOrigData.at(i)->Z()!=fPosz.at(0))
      break;
    if (fOrigData.at(i)->X()>fR[1])
      fR[1] = fOrigData.at(i)->X();
  }

  fR[0] = -fR[1];
}

void TPhitsLoad :: EventLoop(TFile* file, const int ntree)
{
  TTree* tree = (TTree*)file->Get( Form("tree%i", ntree) );
#if defined(DEBUG)
  std::cout << " -- loading the tree: " << tree->GetTitle() << std::endl;
#endif

  int    id  [2];
  double pos [3];
  double flux[2];
  double dose[2];
  double dpa [2];

  // set the information address
  tree->SetBranchAddress(   "idx",   &id[0]);
  tree->SetBranchAddress(   "idy",   &id[1]);
  tree->SetBranchAddress(     "x",  &pos[0]);
  tree->SetBranchAddress(     "y",  &pos[1]);
  tree->SetBranchAddress(   "flx", &flux[0]);
  tree->SetBranchAddress( "flx_e", &flux[1]);
  tree->SetBranchAddress(  "dose", &dose[0]);
  tree->SetBranchAddress("dose_e", &dose[1]);
  tree->SetBranchAddress(   "dpa",  &dpa[0]);
  tree->SetBranchAddress( "dpa_e",  &dpa[1]);

  // get position z
  std::stringstream z( tree->GetTitle()+2 );
  z >> pos[2];
  fPosz.push_back(pos[2]);

  // fill the data into array
  TRadLossContainer* rad = NULL;

  for (int i=0; i<tree->GetEntries(); i++) {
    tree->GetEntry(i);

    rad = new TRadLossContainer;
    rad->SetId(i);
    rad->SetFlux( flux[0], flux[1] );
    rad->SetDose( dose[0], dose[1] );
    rad->SetDpa( dpa[0], dpa[1] );
    rad->SetPosition( pos[0], pos[1], pos[2] );
    fOrigData.push_back(rad);
    //fOrigData.insert( std::pair<double, TRadLossContainer*>(pos[2],rad) );
  }

  //for (auto i=0; i<fOrigData.size(); i++)
  //  std::cout << i << " " << fOrigData.at(i)->GetInfo(kDose)[0] << std::endl;
}

TRadLossContainer* TPhitsLoad :: GetMaximum(const double& z, const Rad opt)
{
  TRadLossContainer* data = new TRadLossContainer;
  double max = -999999999.;
  double pos = -9999.;
  double* info = NULL;

  for (auto i=0; i<fOrigData.size(); i++) {
    pos = fOrigData.at(i)->Z();
    if (pos>z)
      break;
    if (pos==z) {
      info = fOrigData.at(i)->GetInfo(opt);
      //if ( info[0]>max && info[1]<=fTh*info[0] ) {
      if ( info[0]>max && info[1]<=fTh && info[0]>0. ) {
        data = fOrigData.at(i);
        max = info[0];
      }
    }
  }

  return data;
}

TRadLossContainer* TPhitsLoad :: GetAverage(const double& z)
{
  TRadLossContainer* data = new TRadLossContainer;
  double flux[2] = {0., 0.};
  double dose[2] = {0., 0.};
  double dpa [2] = {0., 0.};

  for (auto i=0; i<fOrigData.size(); i++) {
    flux[0] += fOrigData.at(i)->GetFluxData() / static_cast<double>(fOrigData.size());
    dose[0] += fOrigData.at(i)->GetDoseData() / static_cast<double>(fOrigData.size());
    dpa [0] += fOrigData.at(i)->GetDpaData() / static_cast<double>(fOrigData.size());

    flux[1] += pow(fOrigData.at(i)->GetFluxError()*fOrigData.at(i)->GetFluxData(),2);
    dose[1] += pow(fOrigData.at(i)->GetDoseError()*fOrigData.at(i)->GetDoseData(),2);
    dpa [1] += pow(fOrigData.at(i)->GetDpaError()*fOrigData.at(i)->GetDpaData(),2);
  }

  flux[1] = sqrt(flux[1])/static_cast<double>(fOrigData.size());
  dose[1] = sqrt(dose[1])/static_cast<double>(fOrigData.size());
  dpa [1] = sqrt(dpa [1])/static_cast<double>(fOrigData.size());

  data->SetFlux( flux[0], flux[1] );
  data->SetDose( dose[0], dose[1] );
  data->SetDpa ( dpa [0], dpa [1] );
  data->SetPosition( 0., 0., z );

  return data;
}

TRadLossContainer* TPhitsLoad :: GetMinimum(const double& z, const Rad opt)
{
  TRadLossContainer* data = new TRadLossContainer;
  double min = 99999999999.;
  double pos = -99999.;
  double* info = NULL;

  for (auto i=0; i<fOrigData.size(); i++) {
    pos = fOrigData.at(i)->Z();
    if (pos>z)
      break;
    if (pos==z) {
      info = fOrigData.at(i)->GetInfo(opt);
      if ( info[0]<min && info[1]<=fTh && info[0]>0. ) {
        data = fOrigData.at(i);
        min  = info[0];
      }
    }
  }

  return data;
}

double TPhitsLoad :: GetMinValue(const Rad opt)
{
  double min = 1e+30;

  for (auto i=0; i<fOrigData.size(); i++) {
    if (fOrigData.at(i)->GetInfo(opt)[0]<min)
      min = fOrigData.at(i)->GetInfo(opt)[0];
  }

  return min;
}

double TPhitsLoad :: GetMaxValue(const Rad opt)
{
  double max = -9999.;

  for (auto i=0; i<fOrigData.size(); i++) {
    if (fOrigData.at(i)->GetInfo(opt)[0]>max)
      max = fOrigData.at(i)->GetInfo(opt)[0];
  }

  return max;
}

std::map<TRadLossContainer*, TRadLossContainer*> TPhitsLoad :: GetMaxMinLists(const Rad opt)
{
  std::map<TRadLossContainer*, TRadLossContainer*> lim;
  TRadLossContainer* min = NULL;
  TRadLossContainer* max = NULL;

  for (auto i=0; i<fPosz.size(); i++) {
    min = GetMinimum(fPosz.at(i), opt);
    max = GetMaximum(fPosz.at(i), opt);
    lim.insert( std::pair<TRadLossContainer*, TRadLossContainer*>(min,max) );
  }

  return lim;
}

TRadLossContainer* TPhitsLoad :: Resize(const double zmin, const double zmax, 
                                        const double rmin, const double rmax,
                                        const double pmin, const double pmax)
{
  double z, r, p;
  TRadLossContainer* loss = new TRadLossContainer;
  double flux[2] = {0., 0.};
  double dose[2] = {0., 0.};
  double dpa [2] = {0., 0.};
  int cnt = 0;

  for (auto i=0; i<fOrigData.size(); i++) {
    z = fOrigData.at(i)->Z();
    r = fOrigData.at(i)->R();
    p = fOrigData.at(i)->Theta();
    
    if (z>zmax) break;
    
    if ( pmin >= 0.) {
      if ( (z<zmax && z>=zmin) && (r<rmax && r>=rmin) && (p<pmax && p>=pmin) ) {
        flux[0] += fOrigData.at(i)->GetFluxData();
        dose[0] += fOrigData.at(i)->GetDoseData();
        dpa [0] += fOrigData.at(i)->GetDpaData();

        flux[1] += pow(fOrigData.at(i)->GetFluxError(), 2);
        dose[1] += pow(fOrigData.at(i)->GetDoseError(), 2);
        dpa [1] += pow(fOrigData.at(i)->GetDpaError(), 2);

        cnt ++;
      }
    }
    else {
      if ( (z<zmax && z>=zmin) && (r<rmax && r>=rmin) && ((p>=0.&&p<pmax) || (p>=360.+pmin && p<=360.)) ) {
        flux[0] += fOrigData.at(i)->GetFluxData();
        dose[0] += fOrigData.at(i)->GetDoseData();
        dpa [0] += fOrigData.at(i)->GetDpaData();

        flux[1] += pow(fOrigData.at(i)->GetFluxError(), 2);
        dose[1] += pow(fOrigData.at(i)->GetDoseError(), 2);
        dpa [1] += pow(fOrigData.at(i)->GetDpaError(), 2);

        cnt ++;
      }
    }
  }

  flux[0] /= (double)cnt;
  dose[0] /= (double)cnt;
  dpa [0] /= (double)cnt;

  flux[1] = sqrt(flux[1]) / cnt;
  dose[1] = sqrt(dose[1]) / cnt;
  dpa [1] = sqrt(dpa [1]) / cnt;

  loss->SetPosition( zmin+(zmax-zmin)/2., rmin+(rmax-rmin)/2., pmin+(pmax-pmin)/2. );
  loss->SetFlux( flux[0], flux[1] );
  loss->SetDpa ( dpa [0], dpa [1] );
  loss->SetDose( dose[0], dose[1] );

  return loss;
}

std::vector<TRadLossContainer*> TPhitsLoad :: ResizeZR(const double zmin, const double zmax, const int zbin,
                                                       const double rmin, const double rmax, const int rbin,
                                                       const double pmin, const double pmax)
{
  const double dz = (zmax - zmin) / zbin;
  const double dr = (rmax - rmin) / rbin;

  double z = zmin;
  double r = rmin;

  std::vector<TRadLossContainer*> list;
  TRadLossContainer* loss = new TRadLossContainer;

  for (int i=0; i<zbin; i++) {
    z = zmin + i*dz + dz/2.;
    for (int j=0; j<rbin; j++) {
      r = rmin + j*dr + dr/2.;
      loss = Resize( z-dz/2., z+dz/2., r-dr/2., r+dr/2., pmin, pmax );
      loss->SetXyId( i, j );
      list.push_back( loss );
    }
  }

  return list;
}

void TPhitsLoad :: WriteInput(const char* name, const double zmin, const double zmax, const int zbin,
                                                const double rmin, const double rmax, const int rbin,
                                                const double pmin, const double pmax, const int pbin)
{
  const double dz = (zmax - zmin) / zbin;
  const double dr = (rmax - rmin) / rbin;
  const double dp = (pmax - pmin) / pbin;

  std::ofstream output;
  output.open(name);

  double z = zmin;
  double r = rmin;
  double p = pmin;

  TRadLossContainer* loss = new TRadLossContainer;

  for (int i=0; i<rbin; i++) {
    r = rmin + i*dr + dr/2.;

    for (int j=0; j<zbin; j++) {
      z = zmin + j*dz + dz/2.;

      for (int k=0; k<pbin; k++) {
        p = pmin + k*dp + dp/2.;
        loss = Resize( z-dz/2., z+dz/2., r-dr/2., r+dr/2., p-dp/2., p+dp/2. );
        
        output << j << "  " << k << "  " << i << "  " << loss->GetFluxData()*1e+4*365*24*3600. << "  "; 
        output << loss->GetDoseData()*4.4e+13 << "  " << loss->GetDpaData()*1e-24*365*24*3600.*4.4e+13 << "\n";
      }
    }
  }

  output.close();
}

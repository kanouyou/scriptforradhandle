#include <iostream>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TStyle.h>
#include <TH2F.h>
#include "TPhitsPlot.h"

TPhitsPlot :: TPhitsPlot()
    : TPhitsLoad(),
      fWgt(1.)
{
  Setup();
}

TPhitsPlot :: TPhitsPlot(const char* filename)
    : TPhitsLoad(filename),
      fWgt(1.)
{
  Setup();
}

TPhitsPlot :: ~TPhitsPlot()
{}

void TPhitsPlot :: Setup()
{
  gStyle->SetTitleSize(0.045, "xyz");
  gStyle->SetTitleOffset(1.3, "z");
  gStyle->SetLabelSize(0.045, "xyz");
  gStyle->SetOptStat(0);
  gStyle->SetEndErrorSize(0.);
}

TGraphErrors* TPhitsPlot :: GetPlotMax(const Rad opt)
{
  TGraphErrors* gr = new TGraphErrors;

  double max[2] = {0., 0.};
  int cnt = 0;
  std::map<TRadLossContainer*, TRadLossContainer*> list = GetMaxMinLists(opt);
  const double dz = (GetPosEntry(10)-GetPosEntry(9));

  for (auto it=list.begin(); it!=list.end(); ++it) {
    gr->SetPoint( cnt, it->second->Z(), it->second->GetInfo(opt)[0]*fWgt );
    gr->SetPointError( cnt, dz/2., it->second->GetInfo(opt)[1]*it->second->GetInfo(opt)[0]*fWgt );

    if (it->second->GetInfo(opt)[0]*fWgt>max[0]) {
      max[0] = it->second->GetInfo(opt)[0] * fWgt;
      max[1] = it->second->GetInfo(opt)[0] * it->second->GetInfo(opt)[1] * fWgt;
    }
    cnt ++;
  }

  gr->SetMarkerStyle(20);
  gr->SetMarkerColor(kRed);
  gr->SetMarkerSize(0.6);

  std::cout << "max: " << max[0] << " +- " << max[1] << std::endl;

  return gr;
}

TH1F* TPhitsPlot :: GetErrorHist(const Rad opt)
{
  TH1F* hist = new TH1F( Form("err%i",opt), "err", 125, 0.01, 1.);
  std::vector<TRadLossContainer*> data = GetDataLists();

  for (auto i=0; i<data.size(); i++)
    hist->Fill(data.at(i)->GetInfo(opt)[1]);

  return hist;
}

TH2F* TPhitsPlot :: Get2dErrorSpot(const Rad opt, const double zmin, const double zmax)
{
  TH2F* hist = new TH2F( Form("err_spot%i",opt), "errspot", fMesh, fR[0], fR[1], fMesh, fR[0], fR[1] );
  std::vector<TRadLossContainer*> data = GetDataLists();
  double* info = NULL;

  for (auto i=0; i<data.size(); i++) {
    info = data.at(i)->GetInfo(opt);
    if ( info[1]>GetErrThreshold() && data.at(i)->Z()<=zmax && data.at(i)->Z()>zmin )
      hist->Fill( data.at(i)->X(), data.at(i)->Y() );
  }

  return hist;
}


TH2F* TPhitsPlot :: Get2dRZ(const Rad opt, const double zmin, const double zmax, const int zbin,
                                           const double rmin, const double rmax, const int rbin,
                                           const double pmin, const double pmax)
{
  std::vector<TRadLossContainer*> data = ResizeZR(zmin, zmax, zbin, rmin, rmax, rbin, pmin, pmax);

  TH2F* hist = new TH2F( Form("z%.1f-%.1fr%.1f-%.1fp%.1f-%.1f",zmin,zmax,rmin,rmax,pmin,pmax), "r-z",
                         zbin, zmin, zmax, rbin, rmin, rmax );


  for (auto i=0; i<data.size(); i++) {
    std::cout << data.at(i)->IdX() << "  " << data.at(i)->IdY() << "  ";
    std::cout << data.at(i)->X() << "  " << data.at(i)->Y() << "  " << data.at(i)->GetInfo(opt)[0];
    std::cout << "  " << data.at(i)->GetInfo(opt)[1] << std::endl;
    hist->Fill( data.at(i)->X(), data.at(i)->Y(), data.at(i)->GetInfo(opt)[0]*fWgt );
  }

  hist->SetTitle( Form("#phi = %.1f - %.1f [degree]; Z [cm]; R [cm];", pmin, pmax) );
  //hist->Scale( 1./cnt );

  return hist;

}

/*
TH2F* TPhitsPlot :: Get2dRZ(const Rad opt, const double zmin, const double zmax, 
                                           const double rmin, const double rmax,
                                           const double pmin, const double pmax)
{
  const int zbin = 8;
  const int rbin = 10;
  std::vector<TRadLossContainer*> data = GetDataLists();

  TH2F* hist = new TH2F( Form("z%.1f-%.1fr%.1f-%.1fp%.1f-%.1f",zmin,zmax,rmin,rmax,pmin,pmax), "r-z",
                         zbin, zmin, zmax, rbin, rmin, rmax );

  double z = 0.; double r = 0.; double p = 0.;

  for (auto i=0; i<data.size(); i++) {
    z = data.at(i)->Z();
    r = data.at(i)->R();
    p = data.at(i)->Theta();

    if (z>zmax) break;
    
    if ( (z<zmax && z>=zmin) && (r<rmax && r>=rmin) && (p<pmax && p>=pmin) ) 
      hist->Fill( z, r, data.at(i)->GetInfo(opt)[0]*fWgt );
  }

  hist->SetTitle( Form("#phi:%.1f - %.1f degree; Z [cm]; R [cm];", pmin, pmax) );
  //hist->Scale( 1./cnt );

  return hist;
}
*/

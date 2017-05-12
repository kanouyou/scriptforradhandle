
const int      nx = 100;
const double xmin = -80.6;
const double xmax = -xmin;
const int      ny = nx;
const double ymin = xmin;
const double ymax = xmax;

double GetRadius(const double x, const double y)
{ return sqrt(pow(x,2) + pow(y,2)); }

double GetTheta(const double x, const double y) {
  const double r = GetRadius(x, y);
  double theta = acos(x/r);
  if (y<0.)
    theta = 2.*TMath::Pi() - theta;
  return theta * 180. / TMath::Pi();
}

double* FindMax(const char* filename, const int id)
{
  double* max = new double[2];
  max[0] = 0.;
  max[1] = 0.;
  
  TFile* file = new TFile(filename);
  TTree* tree = (TTree*)file->Get( Form("tree%i",id) );

  double pos[2];
  double dep[2];

  tree->SetBranchAddress("x", &pos[0]);
  tree->SetBranchAddress("y", &pos[1]); tree->SetBranchAddress("e", &dep[0]);
  tree->SetBranchAddress("err", &dep[1]);

  for (int i=0; i<tree->GetEntries(); i++) {
    tree->GetEntry(i);
    if (dep[0]>max[0]) {
      max[0] = dep[0];
      max[1] = dep[1];
    }
  }

  return max;
}

TH2F* Load(const char* filename, int id)
{
  TFile* file = new TFile(filename);
  TTree* tree = (TTree*)file->Get( Form("tree%i",id) );

  double pos[2];
  double dep[2];

  tree->SetBranchAddress("x", &pos[0]);
  tree->SetBranchAddress("y", &pos[1]); 
  tree->SetBranchAddress("e", &dep[0]);
  tree->SetBranchAddress("err", &dep[1]);

  TH2F* hist = new TH2F( Form("dep%i",id), Form("dep%i",id), nx, xmin, xmax, 
                                                             ny, ymin, ymax );

  for (int i=0; i<tree->GetEntries(); i++) {
    tree->GetEntry(i);
    hist->Fill(pos[0], pos[1], dep[0]);
  }

  return hist;
}

TH2F* Load2(const char* filename, int id)
{
  TFile* file = new TFile(filename);
  TTree* tree = (TTree*)file->Get( Form("tree%i",id) );

  std::cout << tree->GetEntries() << std::endl;

  double pos[2];
  double dep[2];
  double r, theta;

  tree->SetBranchAddress("x", &pos[0]);
  tree->SetBranchAddress("y", &pos[1]);
  tree->SetBranchAddress("e", &dep[0]);
  tree->SetBranchAddress("err", &dep[1]);

  const int    np   = 10;
  const double pmin = 0.;
  const double pmax = 360.;
  const int    nr   = 10;
  const double rmin = 66.2;
  const double rmax = 80.6;

  TH2F* hist = new TH2F( Form("dep%i",id), Form("dep%i",id), np, pmin, pmax, 
                                                             nr, rmin, rmax );

  for (int i=0; i<tree->GetEntries(); i++) {
    tree->GetEntry(i);
    r     = GetRadius(pos[0], pos[1]);
    theta = GetTheta(pos[0], pos[1]);
    hist->Fill(theta, r, dep[0]);
  }

  return hist;
}

void plot2dedep()
{
  TCanvas* c0 = new TCanvas("c0", "c0", 700, 450);
  c0->Divide(3,2);

  double* max = FindMax("EnergyDepCS0.root", 2);
  std::cout << max[0]*4.4e+13 << " +- " << max[1]*max[0]*4.4e+13 << endl;
  
  /*
  c0->cd(1);
  Load2("EnergyDepCS0.root", 1)->Draw("colz");
  c0->cd(2);
  Load2("EnergyDepCS0.root", 3)->Draw("colz");
  */

  TH2F* hist = NULL;
  for (int i=0; i<5; i++) {
    c0->cd(i+1);
    gPad->SetLogz();
    gPad->SetTicks(1,1);
    hist = Load("EnergyDepCS0.root", i+1);
    hist->Draw("colz");
  }
}

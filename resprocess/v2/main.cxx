#ifndef DEBUG
 #define DEBUG
#endif

#include <iostream>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include "TPhitsPlot.h"

void ShowError(TPhitsPlot* phits)
{
  TH1F* hist[3];
  hist[0] = phits->GetErrorHist(kNeutron);
  hist[1] = phits->GetErrorHist(kDpa);
  hist[2] = phits->GetErrorHist(kDose);
  hist[0]->SetTitle(";Relative Error;Counts");

  TCanvas* c0 = new TCanvas("err", "err", 500, 380);
  c0->SetTicks(1,1);
  c0->SetLogy();
  c0->SetGrid();

  TLegend* lg = new TLegend(0.70, 0.70, 0.89, 0.89);
  lg->SetTextSize(0.045);
  lg->AddEntry(hist[0], "Neutron", "l");
  lg->AddEntry(hist[1], "Dpa", "l");
  lg->AddEntry(hist[2], "Dose", "l");

  for (int i=0; i<3; i++) {
    hist[i]->SetLineColor(i+1);
    if (i==0)
      hist[i]->Draw();
    else
      hist[i]->Draw("same");
  }
  lg->Draw();
}


int main(int argc, char** argv)
{
  const char* filename = argv[1];
  const double pot = 1e+21;
  const double intensity = 4.4e+13;
  TApplication* app = new TApplication("app", &argc, argv);

  TPhitsPlot* phits = new TPhitsPlot;
  phits->Load(filename);
  //phits->SetFactor(4.4e+13 * 1.57e+7 * 1e-24);
  //phits->SetFactor( 1e+4 * pot / intensity );
  //phits->SetFactor( intensity );
  //phits->Get2dRZ(kNeutron, CS1::zmin, CS1::zmax, 10, CS1::rmin, CS1::rmax, 9, 0., 90.)->Draw("colz");
  //phits->SetErrThreshold(0.4);
  //phits->GetPlotMax(kDpa)->Draw("ape");
  //phits->Get2dErrorSpot(kDose)->Draw("colz");

  /*
  phits->SetFactor( 1.57e+7*1e+4 );
  TGraphErrors* gr = phits->GetPlotMax(kNeutron);
  gr->Draw("ape");
  */

  ShowError(phits);

  // write the input file 
  phits->WriteInput("170512cs1x552000.txt", CS1::zmin, CS1::zmax, 10, CS1::rmin, CS1::rmax, 9, 0., 360., 4);
  
  app->Run();

  return 0;
}

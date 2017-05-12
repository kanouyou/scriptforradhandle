#ifndef TPhitsPlot_HH

#include "TPhitsLoad.h"

class TH1F;
class TH2F;

class TPhitsPlot : public TPhitsLoad
{
  public:
    /// constructor
    TPhitsPlot();
    TPhitsPlot(const char* filename);

    /// deconstructor
    ~TPhitsPlot();

    /// setup the factor
    void SetFactor(const double factor) { fWgt = factor; }
    double GetFactor() { return fWgt; }

    /// return the graph of maximum
    TGraphErrors* GetPlotMax(const Rad opt);

    /// return the error hist
    TH1F* GetErrorHist(const Rad opt);

    /// return the histogram
    TH2F* Get2dErrorSpot(const Rad opt, const double zmin=-60., const double zmax=200.);

    /// return the r-z plot
    TH2F* Get2dRZ(const Rad opt, const double zmin, const double zmax, const int zbin,
                                 const double rmin, const double rmax, const int rbin,
                                 const double pmin, const double pmax);


  protected:
    /// setup the plot option
    void Setup();


  private:
    double fWgt;

};

#endif

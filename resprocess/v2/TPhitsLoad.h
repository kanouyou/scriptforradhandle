#ifndef TPhitsLoad_HH

#include <vector>
#include <map>
#include "TRadLossContainer.h"

namespace CS0
{
  const double zmin = 85.788;
  const double zmax = zmin + 18.024;
  const double rmin = 67.20;
  const double rmax = rmin + (1.53+0.15)*9;
}

namespace CS1
{
  const double zmin = -59.525;
  const double zmax = zmin + 139.05;
  const double rmin = 67.20;
  const double rmax = rmin + (1.53+0.15)*9;
}

namespace MS1
{
  const double zmin = -212.1375;
  const double zmax = zmin + 146.775;
  const double rmin = 67.20;
  const double rmax = rmin + (1.53+0.15)*5;
}

namespace MS2
{
  const double zmin = -291.05;
  const double zmax = zmin + 72.10;
  const double rmin = 67.20;
  const double rmax = rmin + (1.53+0.15)*7;
}

class TFile;

class TPhitsLoad
{
  public:
    /// constructor
    TPhitsLoad();
    TPhitsLoad(const char* filename);

    /// deconstructor
    ~TPhitsLoad();

    /// load file
    void Load(const char* filename);

    /// setup the threshold for error
    void SetErrThreshold(const double Th=1.) { fTh = Th; }
    double GetErrThreshold() const { return fTh; }

    /// setup radius cut
    void SetRadius(const double r0, const double r1) { fRmin = r0; fRmax = r1; }
    double GetRmin() const { return fRmin; }
    double GetRmax() const { return fRmax; }

    /// return the data array
    std::vector<TRadLossContainer*> GetDataLists() { return fOrigData; }
    TRadLossContainer* GetDataEntry(const int n) { return fOrigData.at(n); }
    size_t GetDataEntries() const { return fOrigData.size(); }

    /// return the position array
    std::vector<double> GetPosLists() { return fPosz; }
    double GetPosEntry(const int i) { return fPosz.at(i); }

    /// return the class contained the maximum value
    std::map<TRadLossContainer*, TRadLossContainer*> GetMaxMinLists(const Rad opt);

    /// return the maximum and minimum value
    double GetMaxValue(const Rad opt);
    double GetMinValue(const Rad opt);

    /// calculate the average for given mesh
    TRadLossContainer* Resize(const double zmin, const double zmax,
                              const double rmin, const double rmax,
                              const double pmin, const double pmax);

    /// resize the rz mesh and return the lists
    std::vector<TRadLossContainer*> ResizeZR(const double zmin, const double zmax, const int zbin,
                                             const double rmin, const double rmax, const int rbin,
                                             const double pmin, const double pmax);

    /// write down the input for thermal simulation
    void WriteInput(const char* name, const double zmin, const double zmax, const int zbin,
                                      const double rmin, const double rmax, const int rbin,
                                      const double pmin, const double pmax, const int pbin);


  protected:
    /// fill the event into array
    void EventLoop(TFile* file, const int ntree);

    /// calculate the mesh
    void FindMesh();

    /// calculate the maximum and minimum
    TRadLossContainer* GetMaximum(const double& z, const Rad opt);
    TRadLossContainer* GetMinimum(const double& z, const Rad opt);

    /// calculate the average value
    TRadLossContainer* GetAverage(const double& z);

  protected:
    double* fR;
    int     fMesh;
    double  fRmin;
    double  fRmax;

  private:
    //std::map<double, TRadLossContainer*> fOrigData;
    std::vector<TRadLossContainer*> fOrigData;
    std::vector<double> fPosz;
    double fTh;
};

#endif

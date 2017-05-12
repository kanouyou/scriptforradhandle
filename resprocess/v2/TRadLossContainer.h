#ifndef TRadLossContainer_hxx
#define TRadLossContainer_hxx

#include <TObject.h>
#include <TMath.h>

enum Rad
{
  kDose,
  kNeutron,
  kDpa
};

enum XYZ
{
  kR,
  kTheta,
  kX,
  kY,
  kZ
};

class TRadLossContainer
{
  public:
    /// constructor
    TRadLossContainer() : fId(9999999), fIdxy(NULL), fPos(NULL), fInfo(0.), fErr(0.), 
                          fDose(NULL), fFlux(NULL), fDpa(NULL) {
        fPos  = new double[3];
        fIdxy = new int[2];
        fFlux = new double[2];
        fDose = new double[2];
        fDpa  = new double[2];
    }

    /// deconstructor
    ~TRadLossContainer() {
        if (fPos ) delete [] fPos;
        if (fIdxy) delete [] fIdxy;
        if (fFlux) delete [] fFlux;
        if (fDose) delete [] fDose;
        if (fDpa ) delete [] fDpa;
    }

    /// setup id
    void SetId(const int id) { fId = id; }

    /// return id 
    int GetId() const { return fId; }

    /// setup xy id
    void SetXyId(const int idx, const int idy) {
      fIdxy[0] = idx;   fIdxy[1] = idy;
    }

    /// return xy id
    int* GetXyId() const { return fIdxy; }
    int  IdX() const { return fIdxy[0]; }
    int  IdY() const { return fIdxy[1]; }

    /// setup position 
    void SetPosition(const double x, const double y, const double z) {
      fPos[0] = x; fPos[1] = y; fPos[2] = z;
    }

    /// return position
    double* GetPosition() const { return fPos; }
    double  X() const { return fPos[0]; }
    double  Y() const { return fPos[1]; }
    double  Z() const { return fPos[2]; }

    /// coordinate translation
    double R() const { return sqrt(pow(X(),2) + pow(Y(),2)); }
    double Theta() const {
      double theta = acos(X()/R());
      if (Y()<0.) theta = 2.*TMath::Pi() - theta;
      return theta * 180. / TMath::Pi();
    }

    /// setup data
    void SetInfo(const double info) { fInfo = info; }

    /// return data
    double GetInfo() const { return fInfo; }

    /// setup error of data
    void SetError(const double err) { fErr = err; }

    /// return error
    double GetError() const { return fErr; }

    /// setup data
    void SetInfo(const Rad opt, const double &info, const double &err) {
      switch (opt) {
        case kNeutron:
          fFlux[0] = info;
          fFlux[1] = err;
          break;
        case kDose:
          fDose[0] = info;
          fDose[1] = err;
          break;
        case kDpa:
          fDpa[0] = info;
          fDpa[1] = err;
          break;
        default:
          std::cout << "Error: enum option did not exist." << std::endl;
          break;
      }
    }

    /// return the info
    double* GetInfo(const Rad opt) {
      switch (opt) {
        case kNeutron: return fFlux;
        case kDose: return fDose;
        case kDpa : return fDpa;
        default: return NULL;
      }
    }

    /// setup neutron flux
    void SetFlux(const double flx, const double err) {
      fFlux[0] = flx;
      fFlux[1] = err;
    }
    void SetFluxError(const double err) { fFlux[1] = err; }

    /// return neutron flux
    double* GetFlux() const { return fFlux; }
    double  GetFluxData() const { return fFlux[0]; }
    double  GetFluxError() const { return fFlux[1]; }

    /// setup dose
    void SetDose(const double dose, const double err) {
      fDose[0] = dose;
      fDose[1] = err;
    }
    void SetDoseError(const double err) { fDose[1] = err; }

    /// return dose
    double* GetDose() const { return fDose; }
    double  GetDoseData() const { return fDose[0]; }
    double  GetDoseError() const { return fDose[1]; }

    /// setup dpa
    void SetDpa(const double dpa, const double err) {
      fDpa[0] = dpa;
      fDpa[1] = err;
    }
    void SetDpaError(const double err) { fDpa[1] = err; }

    /// return dpa
    double* GetDpa() const { return fDpa; }
    double  GetDpaData() const { return fDpa[0]; }
    double  GetDpaError() const { return fDpa[1]; }


  private:
    int     fId;
    int*    fIdxy;
    double* fPos;
    double  fInfo;
    double  fErr;
    double* fDose;
    double* fFlux;
    double* fDpa;
};

#endif

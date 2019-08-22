#include "wx/wxprec.h"
#include "wx/wx.h"
#include "sample.xpm"
#include <Eigen/Dense>

using namespace Eigen;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxAppHeat
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxAppHeat : public wxApp
{
public:
  virtual bool OnInit();
};

wxIMPLEMENT_APP(wxAppHeat);

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameHeat
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxFrameHeat : public wxFrame
{
public:
  wxFrameHeat(const wxString& title);
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  virtual void OnPaint(wxPaintEvent& event);
  wxColour to_color(float N);

  ~wxFrameHeat()
  {
    delete[] T_fvm;
    delete[] T_fdm_1;
    delete[] Tn_fdm_1;
    delete[] T_fdm_2;
    delete[] Tn_fdm_2;
  }

private:
  void init_constants();
  void solve_fvm();
  void solve_fdm_1();
  void solve_fdm_2();
  void draw_cell(wxDC* dc, float N, wxCoord x, wxCoord y, wxCoord width, wxCoord height);

  //temperature at the left hand side of the bar(deg C)
  float T_A;

  //temperature at the left hand side of the bar(deg C)
  float T_B;

  //thermal conductivity k, [W/mK]
  float k;

  //heat source per unit volume (W/m3)
  float S;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //FVM
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  //number of cells for FVM
  Index m_nbr_fvm;

  //temperature (solution)
  float* T_fvm;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //FDM
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  //number of cells for FVM
  int m_nbr_fdm;

  //temperature (solution)
  float* T_fdm_1;
  float* Tn_fdm_1;

  //temperature (solution)
  float* T_fdm_2;
  float* Tn_fdm_2;

private:
  wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxFrameHeat, wxFrame)
EVT_MENU(wxID_EXIT, wxFrameHeat::OnQuit)
EVT_MENU(wxID_ABOUT, wxFrameHeat::OnAbout)
EVT_PAINT(wxFrameHeat::OnPaint)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxAppHeat::OnInit()
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool wxAppHeat::OnInit()
{
  if (!wxApp::OnInit())
  {
    return false;
  }
  wxFrameHeat* frame = new wxFrameHeat("Heat");
  frame->Maximize(true);
  frame->Show(true);
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameHeat::wxFrameHeat
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxFrameHeat::wxFrameHeat(const wxString& title)
  : wxFrame(NULL, wxID_ANY, title)
{
  SetIcon(wxICON(sample));
  wxMenu* menu_file = new wxMenu;
  menu_file->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");
  wxMenu* menu_help = new wxMenu;
  menu_help->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");
  wxMenuBar* menu_bar = new wxMenuBar();
  menu_bar->Append(menu_file, "&File");
  menu_bar->Append(menu_help, "&Help");
  SetMenuBar(menu_bar);
  CreateStatusBar(2);
  SetStatusText("Ready");
  init_constants();
  solve_fvm();
  solve_fdm_1();
  solve_fdm_2();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameHeat::OnQuit
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameHeat::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameHeat::OnAbout
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameHeat::OnAbout(wxCommandEvent& WXUNUSED(event))
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameHeat::to_color
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxColour wxFrameHeat::to_color(float N)
{
  wxColour C;
  if (N < 130) C = wxColour(187, 206, 255);
  else if (N < 160) C = wxColour(132, 166, 250);
  else if (N < 190) C = wxColour(66, 108, 218);
  else if (N < 200) C = wxColour(50, 72, 141);
  else C = wxColour(34, 47, 87);
  return C;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameHeat::draw_cell
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameHeat::draw_cell(wxDC* dc, float N, wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
  dc->SetBrush(wxBrush(wxColour(to_color(N))));
  dc->DrawRectangle(x, y - height / 2, width, height);
  wxString str = wxString::Format(wxT("%.1f"), N);
  dc->DrawText(str, x + width / 2, y);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameHeat::OnDraw
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameHeat::OnPaint(wxPaintEvent&)
{
  wxPaintDC dc(this);
  wxFont font(wxFontInfo(8).FaceName("Courier"));
  dc.SetFont(font);
  wxCoord width = 100;
  wxCoord height = 50;
  wxCoord x = 0, y = height / 2;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //FVM
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  //boundary
  draw_cell(&dc, T_A, x, y, width, height);
  x += width;
  //interior
  for (int idx = 0; idx < m_nbr_fvm; idx++)
  {
    draw_cell(&dc, T_fvm[idx], x, y, width, height);
    x += width;
  }
  //boundary
  draw_cell(&dc, T_B, x, y, width, height);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //FDM_1
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  x = 0; y += 100;
  for (int idx = 0; idx < m_nbr_fdm; idx++)
  {
    draw_cell(&dc, T_fdm_1[idx], x, y, width, height);
    x += width;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //FDM_2
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  x = 0; y += 100;
  for (int idx = 0; idx < m_nbr_fdm; idx++)
  {
    draw_cell(&dc, T_fdm_2[idx], x, y, width, height);
    x += width;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameHeat::init_constants
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameHeat::init_constants()
{
  //temperature at the left hand side of the bar(deg C)
  T_A = 100;

  //temperature at the left hand side of the bar(deg C)
  T_B = 200;

  //thermal conductivity k, [W/mK]
  k = 100;

  //heat source per unit volume (W/m3)
  S = 1000.0f;;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameHeat::solve_fvm
//Finite Volume Method
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameHeat::solve_fvm()
{
  //number of cells
  m_nbr_fvm = 5;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
 //allocate solution vectors
 /////////////////////////////////////////////////////////////////////////////////////////////////////

 //FVM
  T_fvm = new float[m_nbr_fvm];

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //geometry
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  //length of the bar(m)
  const float length = 5;

  //coordinates of the cell faces
  float* x_faces = new float[m_nbr_fvm + 1];
  x_faces[0] = 0;
  for (int idx = 1; idx < m_nbr_fvm + 1; idx++)
  {
    x_faces[idx] = x_faces[idx - 1] + length / m_nbr_fvm;
    wxLogDebug("x_faces [%d]=%f", idx, x_faces[idx]);
  }

  //coordinates of the cell centroids
  float* x_center = new float[m_nbr_fvm];
  for (int idx = 0; idx < m_nbr_fvm; idx++)
  {
    x_center[idx] = 0.5f * (x_faces[idx + 1] + x_faces[idx]);
    wxLogDebug("x_center [%d]=%f", idx, x_center[idx]);
  }

  //length of each cell
  float* cell_length = new float[m_nbr_fvm];
  for (int idx = 0; idx < m_nbr_fvm; idx++)
  {
    cell_length[idx] = x_faces[idx + 1] - x_faces[idx];
    wxLogDebug("cell_length [%d]=%f", idx, cell_length[idx]);
  }

  //distance between cell centroids
  float* dist_centroids = new float[m_nbr_fvm + 1];
  for (int idx = 0; idx < m_nbr_fvm - 1; idx++)
  {
    dist_centroids[idx] = x_center[idx + 1] - x_center[idx];
    wxLogDebug("dist_centroids [%d]=%f", idx, dist_centroids[idx]);
  }

  //for the boundary cell on the left, the distance is double the distance
  //from the cell centroid to the boundary face
  float dist_left = 2 * (x_center[0] - x_faces[0]);

  //for the boundary cell on the right, the distance is double the distance from
  //the cell centroid to the boundary cell face
  float dist_right = 2 * (x_faces[m_nbr_fvm] - x_center[m_nbr_fvm - 1]);

  for (int idx = 0; idx < m_nbr_fvm; idx++)
  {
    dist_centroids[idx + 1] = dist_centroids[idx];
  }
  dist_centroids[0] = dist_left;
  dist_centroids[m_nbr_fvm] = dist_right;
  for (int idx = 0; idx < m_nbr_fvm + 1; idx++)
  {
    wxLogDebug("dist_centroids [%d]=%f", idx, dist_centroids[idx]);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //constants
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  //cross-sectional area A, [m2]
  const float Area = 0.1f;

  //cell volume
  float* cell_volume = new float[m_nbr_fvm];
  for (int idx = 0; idx < m_nbr_fvm; idx++)
  {
    cell_volume[idx] = cell_length[idx] * Area;
    wxLogDebug("cell_volume [%d]=%f", idx, cell_volume[idx]);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //create the matrices
  //zero initially
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  MatrixXf A = MatrixXf::Zero(m_nbr_fvm, m_nbr_fvm);
  VectorXf B = VectorXf::Zero(m_nbr_fvm);

  for (Index n = 0; n < m_nbr_fvm; n++)
  {
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //left boundary cell
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    if (n == 0)
    {
      //left coefficient aP, T at n
      float aP = k * Area / dist_centroids[n] + k * Area / dist_centroids[n] * 2.0;
      wxLogDebug("left A[%lld,%lld]=%f", n, n, aP);
      A(n, n) = aP;

      //right coefficient aR, T at n+1
      float aR = k * Area / dist_centroids[n];
      wxLogDebug("left A[%lld,%lld]=%f", n, n + 1, -1.0 * aR);
      A(n, n + 1) = -1.0 * aR;

      //source
      float source = S * cell_volume[n];
      //additional boundary source, temperature A
      source += T_A * Area * k / dist_centroids[n] * 2.0;
      wxLogDebug("B[%lld]=%f", n, source);
      B(n) = source;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //right boundary cell
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    else if (n == m_nbr_fvm - 1)
    {
      //left coefficient aL, T at n-1
      float aL = k * Area / dist_centroids[n];;
      wxLogDebug("right A[%lld,%lld]=%f", n, n - 1, -1.0 * aL);
      A(n, n - 1) = -1.0 * aL;

      //right coefficient aP, T at n
      float aP = k * Area / dist_centroids[n] + k * Area / dist_centroids[n] * 2.0;
      wxLogDebug("right A[%lld,%lld]=%f", n, n, aP);
      A(n, n) = aP;

      //source
      float source = S * cell_volume[n];
      //additional boundary source, temperature B
      source += T_B * Area * k / dist_centroids[n] * 2.0;
      wxLogDebug("B[%lld]=%f", n, source);
      B(n) = source;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //interior cells
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    else
    {
      //left coefficient aL, T at n-1
      float aL = k * Area / dist_centroids[n];
      wxLogDebug("A[%lld,%lld]=%f", n, n - 1, -1.0 * aL);
      A(n, n - 1) = -1.0 * aL;

      //right coefficient aR, T at n+1
      float aR = k * Area / dist_centroids[n];
      wxLogDebug("A[%lld,%lld]=%f", n, n + 1, -1.0 * aR);
      A(n, n + 1) = -1.0 * aR;

      //middle coefficient aP, T at n
      float aP = aR + aL;
      wxLogDebug("A[%lld,%lld]=%f", n, n, aP);
      A(n, n) = aP;

      //source
      float source = S * cell_volume[n];
      wxLogDebug("B[%lld]=%f", n, source);
      B(n) = source;
    }
  }

  //solve 
  VectorXf T_vec = A.colPivHouseholderQr().solve(B);
  float* t = T_vec.data();
  for (int idx = 0; idx < m_nbr_fvm; idx++)
  {
    T_fvm[idx] = t[idx];
    wxLogDebug("%.1f", t[idx]);
  }

  delete[] x_faces;
  delete[] x_center;
  delete[] cell_length;
  delete[] dist_centroids;
  delete[] cell_volume;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameHeat::solve_fdm_1
//Finite Differences Method
//diffusion only
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameHeat::solve_fdm_1()
{
  //number of cells
  m_nbr_fdm = 7;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
 //allocate solution vectors
 /////////////////////////////////////////////////////////////////////////////////////////////////////

  //FDM
  T_fdm_1 = new float[m_nbr_fdm];
  Tn_fdm_1 = new float[m_nbr_fdm];

  //time step
  float dt = 0.005f;
  float dx = 1.0f;

  //initial and boundary values
  for (int i = 0; i < m_nbr_fdm; i++)
  {
    T_fdm_1[i] = 0;
  }
  T_fdm_1[0] = T_A;
  T_fdm_1[m_nbr_fdm - 1] = T_B;

  for (int i = 0; i < m_nbr_fdm; i++)
  {
    wxLogDebug("T[%d]=%.1f", i, T_fdm_1[i]);
  }

  //F must be <= 0.5
  float F = k * dt / (dx * dx);
  wxLogDebug("F=%.4f", F);
  for (int n = 0; n < 100; n++)
  {
    for (int i = 1; i < m_nbr_fdm - 1; i++)
    {
      Tn_fdm_1[i] = T_fdm_1[i] + F * (T_fdm_1[i - 1] - 2 * T_fdm_1[i] + T_fdm_1[i + 1]) + dt * S;
    }
    for (int i = 1; i < m_nbr_fdm - 1; i++)
    {
      T_fdm_1[i] = Tn_fdm_1[i];
    }
    for (int i = 0; i < m_nbr_fdm; i++)
    {
      wxLogDebug("T[%d]=%.1f", i, T_fdm_1[i]);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameHeat::solve_fdm_2
//Finite Differences Method
//advection only
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameHeat::solve_fdm_2()
{
  //number of cells
  m_nbr_fdm = 7;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
 //allocate solution vectors
 /////////////////////////////////////////////////////////////////////////////////////////////////////

  //FDM
  T_fdm_2 = new float[m_nbr_fdm];
  Tn_fdm_2 = new float[m_nbr_fdm];

  //time step
  float dt = 0.5f;
  float dx = 1.0f;

  //initial and boundary values
  for (int i = 0; i < m_nbr_fdm; i++)
  {
    T_fdm_2[i] = 0;
  }
  T_fdm_2[0] = T_A;
  T_fdm_2[m_nbr_fdm - 1] = T_B;

  for (int i = 0; i < m_nbr_fdm; i++)
  {
    wxLogDebug("T[%d]=%.1f", i, T_fdm_2[i]);
  }

  int iter = 500;
  float time = dt * (float)iter;
  float U = 1.0f; //velocity
  float C = U * dt / dx; //courant number <= 1
  wxLogDebug("time=%.1f F=%.4f", time, C);
  for (int n = 0; n < iter; n++)
  {
    for (int i = 1; i < m_nbr_fdm - 1; i++)
    {
      Tn_fdm_2[i] = T_fdm_2[i] - C * (T_fdm_2[i] - T_fdm_2[i - 1]);
    }
    for (int i = 1; i < m_nbr_fdm - 1; i++)
    {
      T_fdm_2[i] = Tn_fdm_2[i];
    }
    for (int i = 0; i < m_nbr_fdm; i++)
    {
      wxLogDebug("T[%d]=%.1f", i, T_fdm_2[i]);
    }
  }
}


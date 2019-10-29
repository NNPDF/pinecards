#include "appl_grid/tsparse3d.h"
#include "appl_grid/tsparse2d.h"

template <typename T>
tsparse3d<T>::tsparse3d(int nx, int ny, int nz)
  : tsparse_base(nx), m_Ny(ny), m_Nz(nz), m_v(NULL), m_trimmed(false) {
  m_v = new tsparse2d<T>*[m_Nx];
  for ( int i=0 ; i<m_Nx ; i++ ) m_v[i] = new tsparse2d<T>(m_Ny, m_Nz);
  //   setup_fast();
}

// Fixme: need to rewrite this constructor properly, so that
// a trimmed matrix is copied as trimmed and not copied in full
// and then trimmed
template <typename T>
tsparse3d<T>::tsparse3d(const tsparse3d& t)
  : tsparse_base(t.m_Nx), m_Ny(t.m_Ny), m_Nz(t.m_Nz), m_v(NULL), m_trimmed(t.m_trimmed) {
  m_v = new tsparse2d<T>*[m_Nx];
  for ( int i=0 ; i<m_Nx ; i++ ) m_v[i] = new tsparse2d<T>(m_Ny, m_Nz);
  //   setup_fast();
  // deep copy of all elements
  for ( int i=0 ; i<m_Nx ; i++ ) {
    for ( int j=0 ; j<m_Ny ; j++ ) {
  for ( int k=0 ; k<m_Nz ; k++ ) (*this)(i,j,k) = t(i,j,k);
    }
  }
  if ( m_trimmed ) trim();
}

template <typename T>
tsparse3d<T>::~tsparse3d() {
  if ( m_v ) {
    for ( int i=m_ux-m_lx+1 ; i-- ;  )  if ( m_v[i] ) delete m_v[i];
    delete[] m_v;
  }
  //    empty_fast();
}

template <typename T>
void tsparse3d<T>::trim() {
  m_trimmed = true;

  // trim each column down
  for ( int i=m_ux-m_lx+1 ; i-- ;  )  if ( m_v[i] ) m_v[i]->trim();

  int xmin = m_lx;
  int xmax = m_ux;

  // now find new limits
  for ( ; xmin<xmax+1 && (*m_v[xmin-m_lx])==0 ; xmin++ )  delete m_v[xmin-m_lx];
  for ( ; xmin<xmax   && (*m_v[xmax-m_lx])==0 ; xmax-- )  delete m_v[xmax-m_lx];

  // don't need to change anything
  if ( xmin==m_lx && xmax==m_ux ) return;

  // trimmed matrix is empty
  if ( xmax<xmin ) {
    m_lx = xmin;   m_ux = xmax;
    delete[] m_v;  m_v = NULL;
    return;
  }

  // copy to trimmed matrix (there must be a better way to do this)
  tsparse2d<T>** m_vnew = new tsparse2d<T>*[xmax-xmin+1];
  tsparse2d<T>** mnp = m_vnew;
  tsparse2d<T>** mvp = m_v+xmin-m_lx;
  for ( int j=xmin ; j<=xmax ; j++ )  (*mnp++) = (*mvp++);
  delete[] m_v;

  // update the sparse data and corresponding limits
  m_lx = xmin;
  m_ux = xmax;
  m_v  = m_vnew;
}

template <typename T>
void tsparse3d<T>::untrim() {
  m_trimmed = false;
  grow(0);
  grow(Nx()-1);
  tsparse2d<T>** mvp = m_v;
  for ( int i=m_lx ; i<=m_ux ; i++ ) (*mvp++)->untrim();
}

template <typename T>
bool tsparse3d<T>::trimmed(int i, int j, int k) const {
  if ( i<m_lx || i>m_ux ) return false;
  else                    return m_v[i-m_lx]->trimmed(j,k);
}

template <typename T>
T tsparse3d<T>::operator()(int i, int j, int k) const {
  //  range_check(i);
  if ( i<m_lx || i>m_ux ) return 0;
  return (*((const tsparse2d<T>** const)m_v)[i-m_lx])(j,k);
}

template <typename T>
const tsparse2d<T>* tsparse3d<T>::operator[](int i) const {
  // range_check(i);
  if ( i<m_lx || i>m_ux ) return NULL;
  return m_v[i-m_lx];
}

template <typename T>
T& tsparse3d<T>::operator()(int i, int j, int k) {
  // range_check(i);
  grow(i);
  return (*m_v[i-m_lx])(j,k);
}

template <typename T>
int tsparse3d<T>::size() const {
  int N=0;
  for ( int i=m_ux-m_lx+1 ; i-- ; )  N += m_v[i]->size();
  return N; // +3*sizeof(int);
}

template <typename T>
void tsparse3d<T>::print() const {
  if ( m_ux-m_lx+1==0 ) std::cout << "-" << "\n";
  else {
    for ( int i=0 ; i<Nx() ; i++ ) {
  if ( tsparse_base::trimmed(i) ) {
    std::cout << "m_v[" << i << "]=" << m_v[i-m_lx] << "\n";
    if ( m_v[i-m_lx] ) m_v[i-m_lx]->print();
  }
  else {
    std::cout << "- \t";
  }
  std::cout << "\n";
    }
  }
}

template <typename T>
int tsparse3d<T>::ymin() {
  int minx = m_Ny;
  tsparse2d<T>** mvp = m_v;
  for ( int i=m_lx ; i<=m_ux ; i++ ) {
    int _minx=(*mvp++)->xmin();
    if ( _minx<minx ) minx=_minx;
  }
  return minx;
}

template <typename T>
int tsparse3d<T>::ymax() {
  int maxx = -1;
  tsparse2d<T>** mvp = m_v;
  for ( int i=m_lx ; i<=m_ux ; i++ ) {
    int _minx=(*mvp)->xmin();
    int _maxx=(*mvp++)->xmax();
    if ( _minx<=_maxx && _maxx>maxx ) maxx=_maxx;
  }
  if ( maxx==-1 ) maxx=m_Ny-1;
  return maxx;
}

template <typename T>
int tsparse3d<T>::zmin() {
  int minx = m_Nz;
  tsparse2d<T>** mvp = m_v;
  for ( int i=m_lx ; i<=m_ux ; i++ ) {
    int _minx=(*mvp++)->ymin();
    if ( _minx<minx ) minx=_minx;
  }
  return minx;
}

template <typename T>
int tsparse3d<T>::zmax() {
  int maxx = -1;
  tsparse2d<T>** mvp = m_v;
  for ( int i=m_lx ; i<=m_ux ; i++ ) {
    int _minx=(*mvp)->ymin();
    int _maxx=(*mvp++)->ymax();
    if ( _minx<=_maxx && _maxx>maxx ) maxx=_maxx;
  }
  if ( maxx==-1 ) maxx=m_Nz-1;
  return maxx;
}

template <typename T>
tsparse3d<T>& tsparse3d<T>::operator=(const tsparse3d& t) {
  // clearout what is already in the matrix
  if ( m_v ) {
    for ( int i=m_ux-m_lx+1 ; i-- ; )  if ( m_v[i] ) delete m_v[i];
    delete[] m_v;
    m_v = NULL;
  }

  m_trimmed = t.m_trimmed;

  // now copy everything else
  m_Nx = t.m_Nx;
  m_Ny = t.m_Ny;
  m_Nz = t.m_Nz;

  m_lx = 0;
  m_ux = m_Nx-1;
  m_v = new tsparse2d<T>*[m_Nx];
  for ( int i=0 ; i<m_Nx ; i++ ) m_v[i] = new tsparse2d<T>(m_Ny, m_Nz);
  //   setup_fast();
  // deep copy of all elements
  for ( int i=0 ; i<m_Nx ; i++ ) {
    for ( int j=0 ; j<m_Ny ; j++ ) {
  for ( int k=0 ; k<m_Nz ; k++ ) (*this)(i,j,k) = t(i,j,k);
    }
  }
  if ( m_trimmed ) trim();

  return *this;
}

template <typename T>
tsparse3d<T>& tsparse3d<T>::operator*=(const double& d) {
  for ( int i=0 ; i<Nx() ; i++ ) {
    if ( tsparse_base::trimmed(i) ) {
  if ( m_v[i-m_lx] ) (*m_v[i-m_lx])*=d;
    }
  }
  return *this;
}

template <typename T>
void tsparse3d<T>::grow(int i) {

  // nothing needs to be done;
  if ( i>=m_lx && i<=m_ux ) return;

  // is it empty? add a single row
  if ( m_lx>m_ux ) {
    m_v    = new tsparse2d<T>*[1];
    (*m_v) = new tsparse2d<T>(m_Ny, m_Nz, m_Ny, m_Nz, 0, 0);
    m_lx = m_ux = i;
    return;
  }

  tsparse2d<T>** newv = new tsparse2d<T>*[ ( i<m_lx ? m_ux-i+1 : i-m_lx+1 ) ];
  tsparse2d<T>** newp = newv;
  tsparse2d<T>** mvp  = m_v;

  int lx = m_lx;
  int ux = m_ux;

  // grow at front
  for ( ; m_lx>i ; m_lx-- )  (*newp++) = new tsparse2d<T>(m_Ny, m_Nz, m_Ny, m_Nz, 0, 0);
  // copy existing
  for ( ; lx<=ux ; lx++ )    (*newp++) = (*mvp++);
  // grow at back
  for ( ; m_ux<i ; m_ux++ )  (*newp++) = new tsparse2d<T>(m_Ny, m_Nz, m_Ny, m_Nz, 0, 0);

  delete[] m_v;
  m_v = newv;
}

template class tsparse3d<double>;


#define _UL_GENERATE_CODE_
#include "ssgLocal.h"

sgMat4 _ssgOpenGLAxisSwapMatrix =
{
  {  1.0f,  0.0f,  0.0f,  0.0f },
  {  0.0f,  0.0f, -1.0f,  0.0f },
  {  0.0f,  1.0f,  0.0f,  0.0f },
  {  0.0f,  0.0f,  0.0f,  1.0f }
} ;

sgVec3 _ssgVertex000   = { 0.0f, 0.0f, 0.0f } ;
sgVec4 _ssgColourWhite = { 1.0f, 1.0f, 1.0f, 1.0f } ;
sgVec3 _ssgNormalUp    = { 0.0f, 0.0f, 1.0f } ;
sgVec2 _ssgTexCoord00  = { 0.0f, 0.0f } ;

char *_ssgModelPath   = NULL ;
char *_ssgTexturePath = NULL ;

ssgLight _ssgLights [ 8 ] ;
int      _ssgFrameCounter = 0 ;

int  ssgGetFrameCounter () { return _ssgFrameCounter ; }
void ssgSetFrameCounter ( int fc ) { _ssgFrameCounter = fc ; }

char *ssgGetVersion ()
{
#ifdef VERSION
  return VERSION ;
#else
  return "Unknown" ;
#endif
}

void ssgDeRefDelete ( ssgBase *s )
{
  if ( s == NULL ) return ;

  s -> deRef () ;

  if ( s -> getRef () <= 0 )
    delete s ;
}

void ssgDelete ( ssgBranch *br )
{
  if ( br == NULL )
    return ;

  br -> removeAllKids () ;
  delete br ;
}

ssgLight *ssgGetLight ( int i )
{
  return &_ssgLights [ i ] ;
}

void ssgInit ()
{
#ifdef WIN32
  if ( wglGetCurrentContext () == NULL )
#else
#if defined(macintosh)
  if ( aglGetCurrentContext () == NULL )
#else
  if ( glXGetCurrentContext () == NULL )
#endif
#endif
  {
    ulSetError ( UL_FATAL, "ssgInit called without a valid OpenGL context.");
  }

  ssgTexturePath ( "." ) ;
  ssgModelPath   ( "." ) ;

  _ssgLights [ 0 ] . setID ( 0 ) ;
  _ssgLights [ 0 ] . on    () ;

  for ( int i = 1 ; i < 8 ; i++ )
  {
    _ssgLights [ i ] . setID ( i ) ;
    _ssgLights [ i ] . off   () ;
  }

  new ssgContext ;  /* Sets the current context with defaults */
}


void ssgCullAndPick ( ssgRoot *r, sgVec2 botleft, sgVec2 topright )
{
  if ( _ssgCurrentContext == NULL )
  {
    ulSetError ( UL_FATAL, "ssg: No Current Context: Did you forgot to call ssgInit()?" ) ;
  }

  ssgForceBasicState () ;

  int w = (int)(topright[0] - botleft[0]) ;
  int h = (int)(topright[1] - botleft[1]) ;

  int x = (int)(botleft[0] + topright[0]) / 2 ;
  int y = (int)(botleft[1] + topright[1]) / 2 ;

  GLint viewport [ 4 ] ;
  glGetIntegerv ( GL_VIEWPORT, viewport ) ;
  glMatrixMode ( GL_PROJECTION ) ;
  glLoadIdentity () ;
  gluPickMatrix ( x, y, w, h, viewport ) ;
  _ssgCurrentContext->pushProjectionMatrix () ;

  glMatrixMode ( GL_MODELVIEW ) ;
  glLoadIdentity () ;

  int i ;

  for ( i = 0 ; i < 8 ; i++ )
    if ( _ssgLights [ i ] . isHeadlight () )
      _ssgLights [ i ] . setup () ;

  _ssgCurrentContext->loadModelviewMatrix () ;

  for ( i = 0 ; i < 8 ; i++ )
    if ( ! _ssgLights [ i ] . isHeadlight () )
      _ssgLights [ i ] . setup () ;

  _ssgCurrentContext->cull(r) ;
  _ssgDrawDList () ;

  glMatrixMode ( GL_MODELVIEW ) ;
  glLoadIdentity () ;
}


void ssgCullAndDraw ( ssgRoot *r )
{
  if ( _ssgCurrentContext == NULL )
  {
    ulSetError ( UL_FATAL, "ssg: No Current Context: Did you forgot to call ssgInit()?" ) ;
  }

  _ssgFrameCounter++ ;

  ssgForceBasicState () ;

  glMatrixMode ( GL_PROJECTION ) ;
  _ssgCurrentContext->loadProjectionMatrix () ;

  glMatrixMode ( GL_MODELVIEW ) ;
  glLoadIdentity () ;

  int i ;

  for ( i = 0 ; i < 8 ; i++ )
    if ( _ssgLights [ i ] . isHeadlight () )
      _ssgLights [ i ] . setup () ;

  _ssgCurrentContext->loadModelviewMatrix () ;

  for ( i = 0 ; i < 8 ; i++ )
    if ( ! _ssgLights [ i ] . isHeadlight () )
      _ssgLights [ i ] . setup () ;

  _ssgCurrentContext->cull(r) ;
  _ssgDrawDList () ;

  glMatrixMode ( GL_MODELVIEW ) ;
  glLoadIdentity () ;
}


void ssgModelPath ( char *s )
{
  delete _ssgModelPath ;
  _ssgModelPath = new char [ strlen ( s ) + 1 ] ;
  strcpy ( _ssgModelPath, s ) ;
}

void ssgTexturePath ( char *s )
{
  delete _ssgTexturePath ;
  _ssgTexturePath = new char [ strlen ( s ) + 1 ] ;
  strcpy ( _ssgTexturePath, s ) ;
}


char *ssgBase         ::getTypeName (void) { return "ssgBase"          ; }
char *ssgTexture      ::getTypeName (void) { return "ssgTexture"       ; }
char *ssgState        ::getTypeName (void) { return "ssgState"         ; }
char *ssgSimpleState  ::getTypeName (void) { return "ssgSimpleState"   ; }
char *ssgStateSelector::getTypeName (void) { return "ssgStateSelector" ; }
char *ssgEntity       ::getTypeName (void) { return "ssgEntity"        ; }
char *ssgLeaf         ::getTypeName (void) { return "ssgLeaf"          ; }
char *ssgVTable       ::getTypeName (void) { return "ssgVTable"        ; }
char *ssgVtxTable     ::getTypeName (void) { return "ssgVtxTable"      ; }
char *ssgBranch       ::getTypeName (void) { return "ssgBranch"        ; }
char *ssgSelector     ::getTypeName (void) { return "ssgSelector"      ; }
char *ssgRangeSelector::getTypeName (void) { return "ssgRangeSelector" ; }
char *ssgTimedSelector::getTypeName (void) { return "ssgTimedSelector" ; }
char *ssgBaseTransform::getTypeName (void) { return "ssgBaseTransform" ; }
char *ssgTransform    ::getTypeName (void) { return "ssgTransform"     ; }
char *ssgTexTrans     ::getTypeName (void) { return "ssgTexTrans"      ; }
char *ssgCutout       ::getTypeName (void) { return "ssgCutout"        ; }
char *ssgRoot         ::getTypeName (void) { return "ssgRoot"          ; }


static char *file_extension ( char *fname )
{
  char *p = & ( fname [ strlen(fname) ] ) ;

  while ( p != fname && *p != '/' && *p != '.' )
    p-- ;

  return p ;
}


typedef ssgEntity *_ssgLoader ( char *, ssgHookFunc ) ;
typedef int         _ssgSaver ( char *, ssgEntity * ) ;

struct _ssgFileFormat
{
  char *extension ;
  _ssgLoader *loadfunc ;
  _ssgSaver  *savefunc ;
} ;


static _ssgFileFormat formats[] =
{
  { ".3ds", ssgLoad3ds , NULL       },
  { ".ac" , ssgLoadAC3D, ssgSaveAC  },
  { ".ase", ssgLoadASE , ssgSaveASE },
  { ".dxf", ssgLoadDXF , ssgSaveDXF },
  { ".obj", ssgLoadOBJ , ssgSaveOBJ },
  { ".ssg", ssgLoadSSG , ssgSaveSSG },
  { ".tri", ssgLoadTRI , ssgSaveTRI },
  { ".wrl", ssgLoadVRML, NULL       },
  { NULL  , NULL       , NULL       }
} ;

  
ssgEntity *ssgLoad ( char *fname, ssgHookFunc hookfunc )
{
  if ( fname == NULL || *fname == '\0' )
    return NULL ;

  char *extn = file_extension ( fname ) ;

  if ( *extn != '.' )
  {
    ulSetError ( UL_WARNING, "ssgLoad: Cannot determine file type for '%s'", fname );
    return NULL ;
  }

  for ( _ssgFileFormat *f = formats; f->extension != NULL; f++ )
    if ( f->loadfunc != NULL &&
         _ssgStrNEqual ( extn, f->extension, strlen(f->extension) ) )
      return f->loadfunc( fname, hookfunc ) ;

  ulSetError ( UL_WARNING, "ssgLoad: Unrecognised file type '%s'", extn ) ;
  return NULL ;
}


int ssgSave ( char *fname, ssgEntity *ent )
{
  if ( fname == NULL || ent == NULL || *fname == '\0' )
    return FALSE ;

  char *extn = file_extension ( fname ) ;

  if ( *extn != '.' )
  {
    ulSetError ( UL_WARNING, "ssgSave: Cannot determine file type for '%s'", fname );
    return FALSE ;
  }

  for ( _ssgFileFormat *f = formats; f->extension != NULL; f++ )
    if ( f->savefunc != NULL &&
         _ssgStrNEqual ( extn, f->extension, strlen(f->extension) ) )
      return f->savefunc( fname, ent ) ;

  ulSetError ( UL_WARNING, "ssgSave: Unrecognised file type '%s'", extn ) ;
  return FALSE ;
}



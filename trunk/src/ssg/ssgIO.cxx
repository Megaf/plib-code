
#include "ssgLocal.h" 

static int  read_error = FALSE ;
static int write_error = FALSE ;

static char _ssgAPOM[16*1024]=""; // APOM = actual path of model (!=ModelPath)
static char _ssgANOM[256]=""; // actual name of model

char * ssgGetANOM()
{ return _ssgANOM;
}

char * ssgGetAPOM()
{ return _ssgAPOM;
}

int _ssgReadError  (void) { return  read_error ; }
int _ssgWriteError (void) { return write_error ; }

void _ssgReadFloat ( FILE *fd, float *var )
{
  if ( fread ( var, sizeof(float), 1, fd ) == 1 ) return ;
  read_error = TRUE ;
}


void _ssgWriteFloat ( FILE *fd, const float var )
{
  if ( fwrite ( & var, sizeof(float), 1, fd ) == 1 ) return ;
  write_error = TRUE ;
}



void _ssgReadUInt ( FILE *fd, unsigned int *var )
{
  if ( fread ( var, sizeof(unsigned int), 1, fd ) == 1 ) return ;
  read_error = TRUE ;
}


void _ssgWriteUInt ( FILE *fd, const unsigned int var )
{
  if ( fwrite ( & var, sizeof(unsigned int), 1, fd ) == 1 ) return ;
  write_error = TRUE ;
}


void _ssgReadInt ( FILE *fd, int *var )
{
  if ( fread ( var, sizeof(int), 1, fd ) == 1 ) return ;
  read_error = TRUE ;
}


void _ssgWriteInt ( FILE *fd, const int var )
{
  if ( fwrite ( & var, sizeof(int), 1, fd ) == 1 ) return ;
  write_error = TRUE ;
}


void _ssgReadUShort ( FILE *fd, unsigned short *var )
{
  if ( fread ( var, sizeof(unsigned short), 1, fd ) == 1 ) return ;
  read_error = TRUE ;
}


void _ssgWriteUShort ( FILE *fd, const unsigned short var )
{
  if ( fwrite ( & var, sizeof(unsigned short), 1, fd ) == 1 ) return ;
  write_error = TRUE ;
}


void _ssgReadShort ( FILE *fd, short *var )
{
  if ( fread ( var, sizeof(short), 1, fd ) == 1 ) return ;
  read_error = TRUE ;
}


void _ssgWriteShort ( FILE *fd, const short var )
{
  if ( fwrite ( & var, sizeof(short), 1, fd ) == 1 ) return ;
  write_error = TRUE ;
}


void _ssgReadFloat ( FILE *fd, const unsigned int n, float *var )
{
  if ( fread ( var, sizeof(float), n, fd ) == n ) return ;
  read_error = TRUE ;
}


void _ssgWriteFloat ( FILE *fd, const unsigned int n, const float *var )
{
  if ( fwrite ( var, sizeof(float), n, fd ) == n ) return ;
  write_error = TRUE ;
}

void _ssgReadBytes   ( FILE *fd, const unsigned int n, void *var ) 
{
  if ( n == 0)
		return;
  if ( fread ( var, n, 1, fd ) == 1 ) 
		return ;
  read_error = TRUE ;
}

void _ssgWriteBytes ( FILE *fd, const unsigned int n, const void *var ) 
{
	if ( n == 0)
		return;
  if ( fwrite ( var, n, 1, fd ) == 1 ) 
		return ;
  write_error = TRUE ;
}


void _ssgReadUShort ( FILE *fd, const unsigned int n, unsigned short *var )
{
  if ( fread ( var, sizeof(unsigned short), n, fd ) == n ) return ;
  read_error = TRUE ;
}


void _ssgWriteUShort ( FILE *fd, const unsigned int n, const unsigned short *var )
{
  if ( fwrite ( var, sizeof(unsigned short), n, fd ) == n ) return ;
  write_error = TRUE ;
}



void _ssgReadShort ( FILE *fd, const unsigned int n, short *var )
{
  if ( fread ( var, sizeof(short), n, fd ) == n ) return ;
  read_error = TRUE ;
}


void _ssgWriteShort ( FILE *fd, const unsigned int n, const short *var )
{
  if ( fwrite ( var, sizeof(short), n, fd ) == n ) return ;
  write_error = TRUE ;
}


void _ssgReadUInt ( FILE *fd, const unsigned int n, unsigned int *var )
{
  if ( fread ( var, sizeof(unsigned int), n, fd ) == n ) return ;
  read_error = TRUE ;
}


void _ssgWriteUInt ( FILE *fd, const unsigned int n, const unsigned int *var )
{
  if ( fwrite ( var, sizeof(unsigned int), n, fd ) == n ) return ;
  write_error = TRUE ;
}



void _ssgReadInt ( FILE *fd, const unsigned int n, int *var )
{
  if ( fread ( var, sizeof(int), n, fd ) == n ) return ;
  read_error = TRUE ;
}


void _ssgWriteInt ( FILE *fd, const unsigned int n, const int *var )
{
  if ( fwrite ( var, sizeof(int), n, fd ) == n ) return ;
  write_error = TRUE ;
}



#define MAX_ENTITY_NAME_LENGTH 1024

void _ssgReadString ( FILE *fd, char **var )
{
  int i ;
  char s [ MAX_ENTITY_NAME_LENGTH ] ;

  for ( i = 0 ; i < MAX_ENTITY_NAME_LENGTH ; i++ )
  {
    int c = fgetc ( fd ) ;
    s [ i ] = c ;

    if ( c == '\0' )
      break ;
  }

  if ( i >= MAX_ENTITY_NAME_LENGTH-1 )
    s [ MAX_ENTITY_NAME_LENGTH-1 ] = '\0' ;


  if ( s[0] == '\0' )
    *var = NULL ;
  else
  {
    *var = new char [ strlen(s)+1 ] ;
    strcpy ( *var, s ) ;
  }
}


void _ssgWriteString ( FILE *fd, const char *var )
{
  if ( var != NULL )
    fputs ( var, fd ) ;

  fputc ( '\0', fd ) ;
}


void _ssgReadVec2  ( FILE *fd, sgVec2 var ) { _ssgReadFloat  ( fd, 2, var ) ; }
void _ssgWriteVec2 ( FILE *fd, const sgVec2 var ) { _ssgWriteFloat ( fd, 2, var ) ; }

void _ssgReadVec3  ( FILE *fd, sgVec3 var ) { _ssgReadFloat  ( fd, 3, var ) ; }
void _ssgWriteVec3 ( FILE *fd, const sgVec3 var ) { _ssgWriteFloat ( fd, 3, var ) ; }

void _ssgReadVec4  ( FILE *fd, sgVec4 var ) { _ssgReadFloat  ( fd, 4, var ) ; }
void _ssgWriteVec4 ( FILE *fd, const sgVec4 var ) { _ssgWriteFloat ( fd, 4, var ) ; }

void _ssgReadMat4  ( FILE *fd, sgMat4 var ) { _ssgReadFloat  ( fd, 16, (float *)var ) ; }
void _ssgWriteMat4 ( FILE *fd, const sgMat4 var ) { _ssgWriteFloat ( fd, 16, (float *)var ) ; }


/*
  I'm sick of half the machines on the planet supporting
  strncasecmp and the other half strnicmp - so here is my own
  offering:
*/

int _ssgStrNEqual ( const char *s1, const char *s2, int len )
{
  int l1 = (s1==NULL) ? 0 : strlen ( s1 ) ;
  int l2 = (s2==NULL) ? 0 : strlen ( s2 ) ;

  if ( l1 > len ) l1 = len ;

  if ( l2 < l1 || l1 < len )
    return FALSE ;

  for ( int i = 0 ; i < l1 ; i++ )
  {
    char c1 = s1[i] ;
    char c2 = s2[i] ;

    if ( c1 == c2 )
     continue ;

    if ( c1 >= 'a' && c1 <= 'z' )
      c1 = c1 - ('a'-'A') ;

    if ( c2 >= 'a' && c2 <= 'z' )
      c2 = c2 - ('a'-'A') ;

    if ( c1 != c2 )
     return FALSE ;
  }

  return TRUE ;
}



enum { MAX_SHARED_TEXTURES = 100, MAX_SHARED_STATES = 1000 };
static ssgTexture* shared_textures [ MAX_SHARED_TEXTURES ] ;
static ssgSimpleState* shared_states [ MAX_SHARED_STATES ] ;
static int num_shared_textures = 0 ;
static int num_shared_states = 0 ;

static void _ssgShareReset ()
{
   int i;
   num_shared_textures = 0 ;

   //~~ T.G. we ref() all new states (see below) so we need to deref here
   for ( i = 0; i < num_shared_states; i++)
	   ssgDeRefDelete( shared_states[i] );

   num_shared_states = 0 ;
}

static ssgSimpleState* _ssgShareState ( ssgSimpleState* st )
{
  if ( st == NULL )
     return NULL ;

  for ( int i = 0 ; i < num_shared_states ; i++ )
  {
    ssgSimpleState *st2 = shared_states [ i ] ;

    if ( st == st2 )
      return NULL ; //same pointer -- don't change state

    if ( st->isEnabled ( GL_TEXTURE_2D ) != st2->isEnabled ( GL_TEXTURE_2D ) )
      continue ;

    if ( st->isEnabled ( GL_TEXTURE_2D ) &&
       st -> getTextureHandle () != st2 -> getTextureHandle () )
      continue ;

    if ( st->getCareAbout (SSG_GL_SPECULAR) != st2->getCareAbout (SSG_GL_SPECULAR) ||
      st->getCareAbout (SSG_GL_EMISSION) != st2->getCareAbout (SSG_GL_EMISSION) ||
      st->getCareAbout (SSG_GL_AMBIENT) != st2->getCareAbout (SSG_GL_AMBIENT) ||
      st->getCareAbout (SSG_GL_DIFFUSE) != st2->getCareAbout (SSG_GL_DIFFUSE) )
      continue ;

    if ( ! st->getCareAbout (SSG_GL_SPECULAR) &&
       ! sgEqualVec4 ( st->getMaterial (GL_SPECULAR), st2->getMaterial (GL_SPECULAR) ) )
      continue ;

    if ( ! st->getCareAbout (SSG_GL_EMISSION) &&
       ! sgEqualVec4 ( st->getMaterial (GL_EMISSION), st2->getMaterial (GL_EMISSION) ) )
      continue ;

    if ( ! st->getCareAbout (SSG_GL_AMBIENT) &&
       ! sgEqualVec4 ( st->getMaterial (GL_AMBIENT), st2->getMaterial (GL_AMBIENT) ) )
      continue ;

    if ( ! st->getCareAbout (SSG_GL_DIFFUSE) &&
       ! sgEqualVec4 ( st->getMaterial (GL_DIFFUSE), st2->getMaterial (GL_DIFFUSE) ) )
      continue ;

    if ( st -> isTranslucent () != st2 -> isTranslucent () ||
         st -> getShininess () != st2 -> getShininess () )
      continue ;

    return st2 ;//switch to this state
  }

  //we have a state we've never seen before
  if ( num_shared_states < MAX_SHARED_STATES )
  {
    st -> ref();  // deref'ed in _ssgShareReset()
    shared_states [ num_shared_states++ ] = st ;
  }
  return NULL ; //don't change state
}

ssgLeaf* ssgLoaderOptions::defaultCreateLeaf ( ssgLeaf* leaf,
					       const char* parent_name ) const
{
  /* is this just a sharing 'reset' */
  if ( leaf == NULL )
  {
     _ssgShareReset () ;
     return NULL ;
  }

  /* do we have a texture filename? */
  ssgState* st = leaf -> getState () ;
  if ( st != NULL )
  {
    assert ( st -> isAKindOf ( SSG_TYPE_SIMPLESTATE ) ) ;
    ssgSimpleState *ss = (ssgSimpleState*) st ;

    if ( ss -> getTextureFilename() != NULL ) {
      st = createState( ss -> getTextureFilename() ) ;
      if ( st != NULL ) {
	leaf -> setState( st ) ;
	ss = NULL;
      }
    }

    if (ss != NULL) {
      ss = _ssgShareState ( ss ) ;
      if ( ss != NULL )
	leaf -> setState ( ss ) ;
    }
  }

  return leaf ;
}


ssgTexture* ssgLoaderOptions::defaultCreateTexture ( char* tfname,
						     int wrapu,
						     int wrapv,
						     int mipmap ) const
{
  char filename [ 1024 ] ;

	
  for ( int i = 0 ; i < num_shared_textures ; i++ )
  {
    ssgTexture *tex = shared_textures [ i ] ;
    if ( _ssgStrEqual ( tfname, tex->getFilenameFromModel() ) )
	    return tex ;
  }

	ulFindFile( filename, _ssgTexturePath, tfname, _ssgAPOM ) ;

  ssgTexture* tex = new ssgTexture ( filename, wrapu, wrapv, mipmap ) ;
	tex->setFilenameFromModel ( tfname );
  if ( tex && num_shared_textures < MAX_SHARED_TEXTURES )
     shared_textures [ num_shared_textures++ ] = tex ;
  if ( tex )
    return tex ;
  return 0 ;
}

ssgTransform* ssgLoaderOptions::defaultCreateTransform ( ssgTransform* tr,
      ssgTransformArray* ta ) const
{
  if ( ta != NULL )
    tr -> setUserData ( ta ) ;
  return tr ;
}

ssgLoaderOptions _ssgDefaultOptions ( NULL, NULL, NULL, NULL, NULL ) ;

void ssgModelPath ( const char *s )
{
  delete _ssgModelPath ;
  _ssgModelPath = new char [ strlen ( s ) + 1 ] ;
  strcpy ( _ssgModelPath, s ) ;
}

void ssgTexturePath ( const char *s )
{
  delete _ssgTexturePath ;
  _ssgTexturePath = new char [ strlen ( s ) + 1 ] ;
  strcpy ( _ssgTexturePath, s ) ;
}


static const char *file_extension ( const char *fname )
{
  const char *p = & ( fname [ strlen(fname) ] ) ;

  while ( p != fname && *p != '/' && *p != '.' )
    p-- ;

  return p ;
}


typedef ssgEntity *_ssgLoader ( const char *, const ssgLoaderOptions * ) ;
typedef int         _ssgSaver ( const char *, ssgEntity * ) ;

struct _ssgFileFormat
{
  const char *extension ;
  _ssgLoader *loadfunc ;
  _ssgSaver  *savefunc ;
} ;


static _ssgFileFormat formats[] =
{
  { ".3ds",   ssgLoad3ds  , NULL       },
  { ".ac" ,   ssgLoadAC3D , ssgSaveAC  },
  { ".ase",   ssgLoadASE  , ssgSaveASE },
  { ".dxf",   ssgLoadDXF  , ssgSaveDXF },
  { ".obj",   ssgLoadOBJ  , ssgSaveOBJ },
  { ".ssg",   ssgLoadSSG  , ssgSaveSSG },
  { ".tri",   ssgLoadTRI  , ssgSaveTRI },
  { ".wrl",   ssgLoadVRML , NULL       },
  { ".md2",   ssgLoadMD2  , NULL       },
  { ".mdl",   ssgLoadMDL  , NULL       },
  { ".x"  ,   ssgLoadX    , ssgSaveX   },
  { ".flt",   ssgLoadFLT  , NULL       },
  { ".strip", ssgLoadStrip, NULL       },
  { ".m"  ,   ssgLoadM    , ssgSaveM   },
  { ".off"  , ssgLoadOFF  , ssgSaveOFF },
	{ ".qhi"  , NULL        , ssgSaveQHI },
  { NULL  , NULL       , NULL       }
} ;


	// Changes 14.Feb.2001, Wolfram Kuss:
	// I need this functionality *now* so that I can at last publish my homepage.
	// This code *may* not be very elegant, but should be 
	// portable and works.
	// For more, see mailing list.
	// I/we will decide on a final solution shortly.

#if defined(WIN32) && !defined(__CYGWIN__)
  #include "Shlwapi.h"
	#include "assert.h"
  #define appendPath(a, b) assert( PathAppend(a, b));
#else
//wk stop1

static void appendPath(char *a, char *b)
// appends b to a
{
  if ( b[0] == '/' )
    strcpy ( a, b ) ;  /* b is an absolute path - replace a by b */
	else
		if ( a[0] == 0 )
			strcpy ( a, b ) ;
		else 
			// therefore strlen(a)>0
			if ( a[ strlen(a)-1 ] == '/' )
			{ if ( b[0] == '/' )
					strcat ( a, &(b[1]) );
				else
					strcat ( a, b );
			}
			else
			{ if ( b[0] == '/' )
					strcat ( a, b );
				else
				{ strcat ( a, "/" );
					strcat ( a, b );
				}
			}
}

#endif
  

ssgEntity *ssgLoad ( const char *fname, const ssgLoaderOptions* options )
{
  if ( fname == NULL || *fname == '\0' )
    return NULL ;


	// save path in _ssgAPOM (actual path of model):
#ifdef EXPERIMENTAL_WINDOWS_APOM
	// 14.Feb.2001:
	// This is experimental code by Wolfram Kuss. It runs only on Windows.
	// It will be removed in favour of a final version shortly.

	// start alt + neu1
	strncpy( _ssgAPOM, fname, 1024);
	char *s_ptr;
	s_ptr = &(_ssgAPOM[strlen(_ssgAPOM)-1]);
	while ((s_ptr > _ssgAPOM) && (*s_ptr != '/') && (*s_ptr != '\\')) 
		s_ptr--;
	if ( s_ptr >= _ssgAPOM ) *s_ptr = 0;
	// stop alt + neu1
  // wk start neu1
	char buffer[1024];
  if ( NULL != _fullpath( buffer, _ssgAPOM, 1024 ))
		strncpy( _ssgAPOM, buffer, 1024);
	// stop neu1

#else

	// Changes 14.Feb.2001, Wolfram Kuss:
	// This code accumulates all the relative pathes that make up
	// _ssgAPOM. This code may not be very elegant, but should be 
	// portable and avoid the problems with ssgGetAPOM.
	// For more, see mailing list.
	// I/we will decide on a final solution shortly.

	char buffer[16*1024];
	strncpy( buffer, fname, 16*1024);
	char *s_ptr;
	s_ptr = &(buffer[strlen(buffer)-1]);
	while ((s_ptr > buffer) && (*s_ptr != '/') && (*s_ptr != '\\')) 
		s_ptr--;
// wk: Hint for wk :-): missing in Windows code:

	if ((*s_ptr != '/') && (*s_ptr != '\\'))
	  strcpy(_ssgANOM, s_ptr);
	else
		strcpy(_ssgANOM, &(s_ptr[1]));
	if ( strchr(_ssgANOM, '.') != NULL )
		strchr(_ssgANOM, '.')[0] = 0;
//
	if ( s_ptr >= buffer ) *s_ptr = 0;
	appendPath(_ssgAPOM, buffer);
#endif

	// end of 14.Feb.2001 changes
	


  // find appropiate loader and call its loadfunc
  const char *extn = file_extension ( fname ) ;
  if ( *extn != '.' )
  {
    ulSetError ( UL_WARNING, "ssgLoad: Cannot determine file type for '%s'", fname );
    return NULL ;
  }

  for ( _ssgFileFormat *f = formats; f->extension != NULL; f++ )
    if ( f->loadfunc != NULL &&
	       _ssgStrEqual ( extn, f->extension ) )
      return f->loadfunc( fname, options ) ;

  ulSetError ( UL_WARNING, "ssgLoad: Unrecognised file type '%s'", extn ) ;
  return NULL ;
}


int ssgSave ( const char *fname, ssgEntity *ent )
{
  if ( fname == NULL || ent == NULL || *fname == '\0' )
    return FALSE ;

  const char *extn = file_extension ( fname ) ;

  if ( *extn != '.' )
  {
    ulSetError ( UL_WARNING, "ssgSave: Cannot determine file type for '%s'", fname );
    return FALSE ;
  }

  for ( _ssgFileFormat *f = formats; f->extension != NULL; f++ )
    if ( f->savefunc != NULL &&
         _ssgStrEqual ( extn, f->extension ) )
      return f->savefunc( fname, ent ) ;

  ulSetError ( UL_WARNING, "ssgSave: Unrecognised file type '%s'", extn ) ;
  return FALSE ;
}


char* _ssgMakePath( char* path, const char* dir, const char* fname )
{ // MakePath was originally in ssg, so we dont want to remove this function from ssg
	// But we need it in ul also
	return ulMakePath( path, dir, fname );
}


/*
  I'm sick of half the machines on the planet supporting
  strcasecmp and the other half stricmp - so here is my own
  offering:
*/

int _ssgStrEqual ( const char *s1, const char *s2 )
{
  int l1 = (s1==NULL) ? 0 : strlen ( s1 ) ;
  int l2 = (s2==NULL) ? 0 : strlen ( s2 ) ;

  if ( l1 != l2 ) return FALSE ;

  for ( int i = 0 ; i < l1 ; i++ )
  {
    char c1 = s1[i] ;
    char c2 = s2[i] ;

    if ( c1 == c2 )
     continue ;

    if ( c1 >= 'a' && c1 <= 'z' )
      c1 = c1 - ('a'-'A') ;

    if ( c2 >= 'a' && c2 <= 'z' )
      c2 = c2 - ('a'-'A') ;

    if ( c1 != c2 )
     return FALSE ;
  }

  return TRUE ;
}

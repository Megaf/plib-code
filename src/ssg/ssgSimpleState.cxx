
#include "ssgLocal.h"

void ssgSimpleState::copy_from ( ssgSimpleState *src, int clone_flags )
{
  ssgState::copy_from ( src, clone_flags ) ;

  setTextureFilename ( src -> getTextureFilename () ) ;

  dont_care      = src -> dont_care ;
  enables        = src -> enables   ;
  texture_handle = src -> texture_handle ;

  colour_material_mode = src -> colour_material_mode ;

  sgCopyVec4 ( specular_colour, src -> specular_colour ) ;
  sgCopyVec4 ( emission_colour, src -> emission_colour ) ;
  sgCopyVec4 (  ambient_colour, src ->  ambient_colour ) ;
  sgCopyVec4 (  diffuse_colour, src ->  diffuse_colour ) ;

  shade_model = src -> shade_model ;
  shininess   = src -> shininess   ;
  alpha_clamp = src -> alpha_clamp ;
}

ssgBase *ssgSimpleState::clone ( int clone_flags )
{
  ssgSimpleState *b = new ssgSimpleState ;
  b -> copy_from ( this, clone_flags ) ;
  return b ;
}


void _ssgForceLineState ()
{
  _ssgCurrentContext->getState()->enables &= ~((1<<SSG_GL_TEXTURE_EN) |
                                (1<<SSG_GL_LIGHTING_EN)|
                                (1<<SSG_GL_COLOR_MATERIAL_EN)) ;
  glDisable ( GL_TEXTURE_2D ) ;
  glDisable ( GL_COLOR_MATERIAL ) ;
  glDisable ( GL_LIGHTING  ) ;
  glDisable ( GL_DEPTH_TEST ) ;
}


ssgSimpleState::ssgSimpleState ( int /* I_am_currstate */ )
{
  type |= SSG_TYPE_SIMPLESTATE ;
  filename  = NULL ;
  dont_care = 0xFFFFFFFF ;
  disable ( GL_TEXTURE_2D  ) ;
  enable  ( GL_CULL_FACE   ) ;
  enable  ( GL_COLOR_MATERIAL ) ;
  enable  ( GL_LIGHTING ) ;
  disable ( GL_BLEND ) ;
  enable  ( GL_ALPHA_TEST ) ;
  setShadeModel ( GL_SMOOTH ) ;
}

ssgSimpleState::ssgSimpleState (void)
{
  type |= SSG_TYPE_SIMPLESTATE ;

  filename  = NULL ;
  dont_care = 0xFFFFFFFF ;
}

ssgSimpleState::~ssgSimpleState (void)
{
  delete filename ;
}

void ssgSimpleState::apply (void)
{
  int turn_off = ~dont_care & ~enables & _ssgCurrentContext->getState()->enables & 0x3F ;
  (*(__ssgDisableTable[turn_off]))() ;
  _ssgCurrentContext->getState()->enables &= ~turn_off ;

  if ( ~ dont_care & ( (1<<SSG_GL_COLOR_MATERIAL_EN) |
		       (1<<SSG_GL_COLOR_MATERIAL   ) ) )
  {
    int indeterminate = turn_off & (1<<SSG_GL_COLOR_MATERIAL_EN) ;

    if ( ~ dont_care & (1<<SSG_GL_COLOR_MATERIAL ) &&
	 _ssgCurrentContext->getState()->colour_material_mode != colour_material_mode )
    {
      glColorMaterial ( GL_FRONT_AND_BACK, (GLenum) colour_material_mode ) ;
      _ssgCurrentContext->getState()->colour_material_mode = colour_material_mode ;
      indeterminate |= _ssgCurrentContext->getState()->enables & (1<<SSG_GL_COLOR_MATERIAL_EN) ;
    }

    if ( indeterminate )
    {
      /* invalidate the material colours */
      sgSetVec3 ( _ssgCurrentContext->getState()->specular_colour, -1.0f, -1.0f, -1.0f ) ;
      sgSetVec3 ( _ssgCurrentContext->getState()->emission_colour, -1.0f, -1.0f, -1.0f ) ;
      sgSetVec3 ( _ssgCurrentContext->getState()->ambient_colour, -1.0f, -1.0f, -1.0f ) ;
      sgSetVec4 ( _ssgCurrentContext->getState()->diffuse_colour, -1.0f, -1.0f, -1.0f, -1.0f ) ;
    }
  }

  if ( ~ dont_care & ( (1<<SSG_GL_DIFFUSE       ) |
                       (1<<SSG_GL_AMBIENT       ) |
                       (1<<SSG_GL_SPECULAR      ) |
                       (1<<SSG_GL_EMISSION      ) |
                       (1<<SSG_GL_SHININESS     ) ) )
  {
    if ( ( ~ dont_care & (1<<SSG_GL_SHININESS) ) &&
      _ssgCurrentContext->getState()->shininess != shininess )
    {
      glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, shininess ) ;
      _ssgCurrentContext->getState()->shininess = shininess ;
    }

    if ( ( ~ dont_care & (1<<SSG_GL_SPECULAR) ) &&
         ! sgEqualVec3 ( _ssgCurrentContext->getState()->specular_colour, specular_colour ) )
    {
      glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR, specular_colour ) ;
      sgCopyVec3 ( _ssgCurrentContext->getState()->specular_colour, specular_colour ) ;
    }

    if ( ( ~ dont_care & (1<<SSG_GL_EMISSION) ) &&
         ! sgEqualVec3 ( _ssgCurrentContext->getState()->emission_colour, emission_colour ) )
    {
      glMaterialfv ( GL_FRONT_AND_BACK, GL_EMISSION, emission_colour ) ;
      sgCopyVec3 ( _ssgCurrentContext->getState()->emission_colour, emission_colour ) ;
    }

    if ( ( ~ dont_care & (1<<SSG_GL_AMBIENT) ) &&
         ! sgEqualVec3 ( _ssgCurrentContext->getState()->ambient_colour, ambient_colour ) )
    {
      glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT, ambient_colour ) ;
      sgCopyVec3 ( _ssgCurrentContext->getState()->ambient_colour, ambient_colour ) ;
    }

    if ( ( ~ dont_care & (1<<SSG_GL_DIFFUSE) ) &&
         ! sgEqualVec4 ( _ssgCurrentContext->getState()->diffuse_colour, diffuse_colour ) )
    {
      glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_colour ) ;
      sgCopyVec4 ( _ssgCurrentContext->getState()->diffuse_colour, diffuse_colour ) ;
    }
  }

  if ( ( ~ dont_care & (1<<SSG_GL_TEXTURE) ) && 
    _ssgCurrentContext->getState()->texture_handle != texture_handle )
  {
    stats_bind_textures++ ;
#ifdef GL_VERSION_1_1
    glBindTexture ( GL_TEXTURE_2D, texture_handle ) ;
#else
    /* For ancient SGI machines */
    glBindTextureEXT ( GL_TEXTURE_2D, texture_handle ) ;
#endif
    _ssgCurrentContext->getState()->texture_handle = texture_handle ;
  }

  if ( ( ~ dont_care & (1<<SSG_GL_SHADE_MODEL) ) &&
    _ssgCurrentContext->getState()->shade_model != shade_model )
  {
    glShadeModel ( shade_model ) ;
    _ssgCurrentContext->getState()->shade_model = shade_model ;
  }

  if ( ( ~ dont_care & (1<<SSG_GL_ALPHA_TEST) ) &&
    _ssgCurrentContext->getState()->alpha_clamp != alpha_clamp )
  {
    glAlphaFunc ( GL_GREATER, alpha_clamp ) ;
    _ssgCurrentContext->getState()->alpha_clamp = alpha_clamp ;
  }

  int turn_on = ~dont_care & enables & ~_ssgCurrentContext->getState()->enables & 0x3F ;
  (*(__ssgEnableTable [turn_on ]))() ;
  _ssgCurrentContext->getState()->enables |= turn_on ;
}

void ssgSimpleState::force (void)
{
  int turn_off = ~dont_care & ~enables & 0x3F ;
  (*(__ssgDisableTable[turn_off]))() ;
  _ssgCurrentContext->getState()->enables &= ~turn_off ;

  if ( ~ dont_care & ( (1<<SSG_GL_COLOR_MATERIAL ) |
                       (1<<SSG_GL_DIFFUSE        ) |
                       (1<<SSG_GL_AMBIENT        ) |
                       (1<<SSG_GL_SPECULAR       ) |
                       (1<<SSG_GL_EMISSION       ) |
                       (1<<SSG_GL_SHININESS      ) ) )
  {
    if ( ~ dont_care & (1<<SSG_GL_COLOR_MATERIAL ) )
    {
      glColorMaterial ( GL_FRONT_AND_BACK, (GLenum) colour_material_mode ) ;
      _ssgCurrentContext->getState()->colour_material_mode = colour_material_mode ;
    }

    if ( ~ dont_care & (1<<SSG_GL_SHININESS ) )
    {
      glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, shininess ) ;
      _ssgCurrentContext->getState()->shininess = shininess ;
    }

    if ( ~ dont_care & (1<<SSG_GL_DIFFUSE ) )
    {
      glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_colour ) ;
      sgCopyVec4 ( _ssgCurrentContext->getState()->diffuse_colour, diffuse_colour ) ;
    }

    if ( ~ dont_care & (1<<SSG_GL_EMISSION ) )
    {
      glMaterialfv ( GL_FRONT_AND_BACK, GL_EMISSION, emission_colour ) ;
      sgCopyVec3 ( _ssgCurrentContext->getState()->emission_colour, emission_colour ) ;
    }

    if ( ~ dont_care & (1<<SSG_GL_AMBIENT ) )
    {
      glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT, ambient_colour ) ;
      sgCopyVec3 ( _ssgCurrentContext->getState()->ambient_colour, ambient_colour ) ;
    }

    if ( ~ dont_care & (1<<SSG_GL_SPECULAR ) )
    {
      glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR, specular_colour ) ;
      sgCopyVec3 ( _ssgCurrentContext->getState()->specular_colour, specular_colour ) ;
    }
  }

  if ( ~ dont_care & (1<<SSG_GL_TEXTURE ) )
  {
    stats_bind_textures++ ;
#ifdef GL_VERSION_1_1
    glBindTexture ( GL_TEXTURE_2D, texture_handle ) ;
#else
    /* For ancient SGI machines */
    glBindTextureEXT ( GL_TEXTURE_2D, texture_handle ) ;
#endif
    _ssgCurrentContext->getState()->texture_handle = texture_handle ;
  }

  if ( ~ dont_care & (1<<SSG_GL_SHADE_MODEL ) )
  {
    glShadeModel ( shade_model ) ;
    _ssgCurrentContext->getState()->shade_model = shade_model ;
  }

  if ( ~ dont_care & (1<<SSG_GL_ALPHA_TEST ) )
  {
    glAlphaFunc ( GL_GREATER, alpha_clamp ) ;
    _ssgCurrentContext->getState()->alpha_clamp = alpha_clamp ;
  }

  int turn_on = ~dont_care & enables & 0x3F ;
  (*(__ssgEnableTable [turn_on ]))() ;
  _ssgCurrentContext->getState()->enables |= turn_on ;
}



int ssgSimpleState::isEnabled ( GLenum mode )
{
  switch ( mode )
  {
    case GL_TEXTURE_2D        :
      return enables & (1<<SSG_GL_TEXTURE_EN) ;

    case GL_CULL_FACE      :
      return enables & (1<<SSG_GL_CULL_FACE_EN) ;

    case GL_COLOR_MATERIAL :
      return enables & (1<<SSG_GL_COLOR_MATERIAL_EN) ;

    case GL_LIGHTING :
      return enables & (1<<SSG_GL_LIGHTING_EN) ;

    case GL_BLEND          :
      return enables & (1<<SSG_GL_BLEND_EN) ;

    case GL_ALPHA_TEST     :
      return enables & (1<<SSG_GL_ALPHA_TEST_EN) ;

    default :
      return FALSE ;
  }
}



void ssgSimpleState::disable ( GLenum mode )
{
  switch ( mode )
  {
    case GL_TEXTURE_2D        :
      enables &= ~ (1<<SSG_GL_TEXTURE_EN) ;
      care_about ( SSG_GL_TEXTURE_EN ) ;
      break ;

    case GL_CULL_FACE      :
      enables &= ~ (1<<SSG_GL_CULL_FACE_EN) ;
      care_about ( SSG_GL_CULL_FACE_EN ) ;
      break ;

    case GL_COLOR_MATERIAL :
      enables &= ~ (1<<SSG_GL_COLOR_MATERIAL_EN) ;
      care_about ( SSG_GL_COLOR_MATERIAL_EN ) ;
      break ;

    case GL_LIGHTING :
      enables &= ~ (1<<SSG_GL_LIGHTING_EN) ;
      care_about ( SSG_GL_LIGHTING_EN ) ;
      break ;

    case GL_BLEND          :
      enables &= ~ (1<<SSG_GL_BLEND_EN) ;
      care_about ( SSG_GL_BLEND_EN ) ;
      break ;

    case GL_ALPHA_TEST     :
      enables &= ~ (1<<SSG_GL_ALPHA_TEST_EN) ;
      care_about ( SSG_GL_ALPHA_TEST_EN ) ;
      break ;

    default :
      ulSetError ( UL_WARNING, "Illegal mode passed to ssgSimpleState::disable(%d)",
			       mode ) ;
      break ; 
  }
}

void ssgSimpleState::enable  ( GLenum mode )
{
  switch ( mode )
  {
    case GL_TEXTURE_2D        :
      enables |=  (1<<SSG_GL_TEXTURE_EN) ;
      care_about ( SSG_GL_TEXTURE_EN ) ;
      break ;

    case GL_CULL_FACE      :
      enables |=  (1<<SSG_GL_CULL_FACE_EN) ;
      care_about ( SSG_GL_CULL_FACE_EN ) ;
      break ;

    case GL_COLOR_MATERIAL :
      enables |=  (1<<SSG_GL_COLOR_MATERIAL_EN) ;
      care_about ( SSG_GL_COLOR_MATERIAL_EN ) ;
      break ;

    case GL_BLEND          :
      enables |=  (1<<SSG_GL_BLEND_EN) ;
      care_about ( SSG_GL_BLEND_EN ) ;
      break ;

    case GL_ALPHA_TEST     :
      enables |=  (1<<SSG_GL_ALPHA_TEST_EN) ;
      care_about ( SSG_GL_ALPHA_TEST_EN ) ;
      break ;

    case GL_LIGHTING :
      enables |= (1<<SSG_GL_LIGHTING_EN) ;
      care_about ( SSG_GL_LIGHTING_EN ) ;
      break ;

    default :
      ulSetError ( UL_WARNING,
	     "Illegal mode passed to ssgSimpleState::enable(%d)",
			       mode ) ;
      break ; 
  }
}


int ssgSimpleState::load ( FILE *fd )
{
  delete filename ;
	int    wrapu, wrapv ;

  _ssgReadInt   ( fd, & dont_care            ) ;
  _ssgReadInt   ( fd, & enables              ) ;
  _ssgReadString( fd, & filename             ) ;
  _ssgReadInt   ( fd, & wrapu                ) ;
  _ssgReadInt   ( fd, & wrapv                ) ;
  _ssgReadInt   ( fd, & colour_material_mode ) ;
  _ssgReadVec4  ( fd, specular_colour        ) ;
  _ssgReadVec4  ( fd, emission_colour        ) ;
  _ssgReadVec4  ( fd, ambient_colour         ) ;
  _ssgReadVec4  ( fd, diffuse_colour         ) ;
  _ssgReadInt   ( fd, (int *)(& shade_model) ) ;
  _ssgReadFloat ( fd, & shininess            ) ;
  _ssgReadFloat ( fd, & alpha_clamp          ) ;

  if ( filename != NULL && filename[0] != '\0' )
    setTexture ( filename, wrapu, wrapv ) ;
  else
    texture_handle = 0 ;

  return ssgState::load(fd) ;
}


int ssgSimpleState::save ( FILE *fd )
{
  int    wrapu = TRUE /*getWrapU()*/, wrapv = TRUE /*getWrapV()*/; // FIXME, TODO

  _ssgWriteInt   ( fd, dont_care            ) ;
  _ssgWriteInt   ( fd, enables              ) ;
#ifdef WRITE_SSG_VERSION_ZERO
	// change *.?af filename into *?.bmp, for example catalina.3af into catalina3.bmp
	if ( (filename != NULL) &&
		   (strlen(filename) > 2) && 
		   ((filename[strlen(filename)-2] == 'a') || (filename[strlen(filename)-2] == 'A')) &&
		   ((filename[strlen(filename)-1] == 'f') || (filename[strlen(filename)-1] == 'F')))
	{
		char *myfilename = new char [strlen(filename)+20];
		strcpy(myfilename, filename);
		char * ptr;
		ptr = &(myfilename[strlen(myfilename)-4]); // points to the '.' 
		ptr[0] = ptr[1];
		*++ptr='.'; *++ptr='b'; *++ptr='m'; *++ptr='p'; *++ptr=0;

		_ssgWriteString( fd, myfilename             ) ;
		delete [] myfilename;
	}
	else
		_ssgWriteString( fd, filename             ) ;
#else
  _ssgWriteString( fd, filename             ) ;
#endif
  _ssgWriteInt   ( fd, wrapu                ) ;
  _ssgWriteInt   ( fd, wrapv                ) ;
  _ssgWriteInt   ( fd, colour_material_mode ) ;
  _ssgWriteVec4  ( fd, specular_colour      ) ;
  _ssgWriteVec4  ( fd, emission_colour      ) ;
  _ssgWriteVec4  ( fd, ambient_colour       ) ;
  _ssgWriteVec4  ( fd, diffuse_colour       ) ;
  _ssgWriteInt   ( fd, (int) shade_model    ) ;
  _ssgWriteFloat ( fd, shininess            ) ;
  _ssgWriteFloat ( fd, alpha_clamp          ) ;

  return ssgState::save(fd) ;
}

void ssgSimpleState::setTextureFilename ( const char *fname )
{
  delete filename ;

  if ( fname == NULL )
    filename = NULL ;
  else
  {
    filename = new char [ strlen(fname)+1 ] ;
    strcpy ( filename, fname ) ;
  }
}

/*
wk: not implemented yet
Is ssgSimpleState the right place to do this?
use gkGetTexParamiv or so.

int ssgSimpleState::getWrapU()
{
int    wrapu = TRUE, wrapv = TRUE ;

	if ( filename == NULL )
		return TRUE;
	if ( filename[0] == 0 )
		return TRUE;
	GLuint texture_handle = getTextureHandle ();
	
}
*/

void ssgSimpleState::setTexture ( char *fname, int _wrapu, int _wrapv,
				  int _mipmap )
{
  mipmap = _mipmap ;
  ssgTexture *tex = new ssgTexture ( fname, _wrapu, _wrapv, mipmap ) ;
  setTexture ( tex ) ;
  delete tex ;
}


 
static void printStateString ( FILE *fd, unsigned int bits )
{
  if ( bits & (1<<SSG_GL_TEXTURE_EN) ) fprintf ( fd, "TEXTURE2D " ) ;
  if ( bits & (1<<SSG_GL_CULL_FACE_EN) ) fprintf ( fd, "CULLFACE " ) ;
  if ( bits & (1<<SSG_GL_COLOR_MATERIAL_EN) ) fprintf ( fd, "COLOR_MATERIAL ");
  if ( bits & (1<<SSG_GL_BLEND_EN) ) fprintf ( fd, "BLEND " ) ;
  if ( bits & (1<<SSG_GL_ALPHA_TEST_EN) ) fprintf ( fd, "ALPHA_TEST " ) ;
  if ( bits & (1<<SSG_GL_LIGHTING_EN) ) fprintf ( fd, "LIGHTING " ) ;
}

 
void ssgSimpleState::print ( FILE *fd, char *indent, int how_much )
{
  ssgState::print ( fd, indent, how_much ) ;
 
	if ( how_much < 2 )
		return;

  fprintf ( fd, "%s  Don't Care   = ", indent ) ;
             printStateString ( fd, dont_care ) ;
             fprintf ( fd, "\n" ) ;
  fprintf ( fd, "%s  Enabled      = ", indent ) ;
             printStateString ( fd, enables ) ;
             fprintf ( fd, "\n" ) ;

  fprintf ( fd, "%s  TexHandle    = %d\n", indent, texture_handle ) ;
  fprintf ( fd, "%s  TexFilename  = '%s'\n", indent,
                           (filename==NULL) ? "<none>" : filename ) ;
  fprintf ( fd, "%s  Shade Model  = %d\n", indent, shade_model ) ;
  fprintf ( fd, "%s  Shininess    = %f\n", indent, shininess ) ;
  fprintf ( fd, "%s  AlphaClamp   = %f\n", indent, alpha_clamp ) ;
  fprintf ( fd, "%s  ColourMatMode= %s\n", indent,
    (colour_material_mode == GL_AMBIENT) ? "GL_AMBIENT" :
    (colour_material_mode == GL_DIFFUSE) ? "GL_DIFFUSE" :
    (colour_material_mode == GL_AMBIENT_AND_DIFFUSE) ? "GL_AMBIENT_AND_DIFFUSE" :
    (colour_material_mode == GL_SPECULAR) ? "GL_SPECULAR" :
    (colour_material_mode == GL_EMISSION) ? "GL_EMISSION" : "?????" ) ;

  fprintf ( fd, "%s  Ambient  : (%f,%f,%f,%f)\n", indent,
                             ambient_colour[0],  ambient_colour[1],
                             ambient_colour[2],  ambient_colour[3] ) ;
  fprintf ( fd, "%s  Diffuse  : (%f,%f,%f,%f)\n", indent,
                             diffuse_colour[0],  diffuse_colour[1],
                             diffuse_colour[2],  diffuse_colour[3] ) ;
  fprintf ( fd, "%s  Specular : (%f,%f,%f,%f)\n", indent,
                            specular_colour[0], specular_colour[1],
                            specular_colour[2], specular_colour[3] ) ;
  fprintf ( fd, "%s  Emission : (%f,%f,%f,%f)\n", indent,
                            emission_colour[0], emission_colour[1],
                            emission_colour[2], emission_colour[3] ) ;
}

// ***********************************************************************
// ******************** class CGlobalSimpleStateList *********************
// ***********************************************************************
 

int CGlobalSimpleStateList::find_state ( ssgState* st )
{
  for ( int i=0; i<num_states; i++ )
    if ( states[ i ] == st )
      return i ;
  return -1 ;
}


void CGlobalSimpleStateList::get_states ( ssgEntity *e )
{
	if ( (states != NULL ) || ( num_states != 0 ) )
    ulSetError( UL_WARNING, "There are already states!\n");
	states = new ssgSimpleState*[ MAX_STATES ] ;
	num_states = 0;

	get_states_recursive (e);
}

void CGlobalSimpleStateList::get_states_recursive ( ssgEntity *e )
{
  if ( e -> isAKindOf ( SSG_TYPE_BRANCH ) )
  {
    ssgBranch *br = (ssgBranch *) e ;

    for ( int i = 0 ; i < br -> getNumKids () ; i++ )
      get_states_recursive ( br -> getKid ( i ) ) ;
  }
  else
  if ( e -> isAKindOf ( SSG_TYPE_VTXTABLE ) )
  {
    ssgVtxTable* vt = (ssgVtxTable *) e ;
    ssgState* st = vt -> getState () ;
    if ( st && st -> isAKindOf ( SSG_TYPE_SIMPLESTATE ) )
    {
      ssgSimpleState* ss = (ssgSimpleState*) vt -> getState () ;
      if ( find_state ( ss ) == -1 )
        states[ num_states++ ] = ss ;
    }
  }
}


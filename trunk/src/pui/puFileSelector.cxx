/*
     PLIB - A Suite of Portable Game Libraries
     Copyright (C) 2001  Steve Baker
 
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
 
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
 
     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the Free
     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 
     For further information visit http://plib.sourceforge.net

     $Id$
*/

/****
* NAME
*   puFileSelector
*
* DESCRIPTION
*   PUI dialog for selecting files
*
* AUTHOR
*   Dave McClurg <dpm@efn.org>
*
* CREATION DATE
*   August 2000
*
* MODIFICATION HISTORY
*   John Fay    - many improvements
*   Steve Baker - moved from FilePicker to FileSelector due to
*                 key API change.
****/


#include "puLocal.h"

#if defined(WIN32) && !defined(__CYGWIN__)
#define DOTDOTSLASH "..\\"
#define SLASH       "\\"
#else
#define DOTDOTSLASH "../"
#define SLASH       "/"
#endif

static void puFileSelectorHandleSlider ( puObject * slider )
{
  float val ;
  slider -> getValue ( &val ) ;
  val = 1.0f - val ;

  puListBox* list_box = (puListBox*) slider -> getUserData () ;
  int index = int ( list_box -> getNumItems () * val ) ;
  list_box -> setTopItem ( index ) ;
}

static void puFileSelectorHandleArrow ( puObject *arrow )
{
  puSlider *slider = (puSlider *) arrow->getUserData () ;
  puListBox* list_box = (puListBox*) slider -> getUserData () ;

  int type = ((puArrowButton *)arrow)->getArrowType() ;
  int inc = ( type == PUARROW_DOWN     ) ?   1 :
            ( type == PUARROW_UP       ) ?  -1 :
            ( type == PUARROW_FASTDOWN ) ?  10 :
            ( type == PUARROW_FASTUP   ) ? -10 : 0 ;

  float val ;
  slider -> getValue ( &val ) ;
  val = 1.0f - val ;
  int num_items = list_box->getNumItems () - 1 ;
  if ( num_items > 0 )
  {
    int index = int ( num_items * val + 0.5 ) + inc ;
    if ( index > num_items ) index = num_items ;
    if ( index < 0 ) index = 0 ;

    slider -> setValue ( 1.0f - (float)index / num_items ) ;
    list_box -> setTopItem ( index ) ;
  }
}


static bool is_slash ( char ch )
{
#if defined(WIN32)
  /* win32 can use back or forward slash */
  return ch == '\\' || ch == '/' ;
#else
  return ch == SLASH[0] ;
#endif
}


static void chop_file ( char *fname )
{
  /* removes everything back to the last '/' */

  for ( int i = strlen(fname)-1 ; ! is_slash ( fname[i] ) && i >= 0 ; i-- )
    fname[i] = '\0' ;
}


static void go_up_one_directory ( char *fname )
{
  /* removes everything back to the last but one '/' */

  chop_file ( fname ) ;

  if ( fname[0] == '\0' )
  {
    /* Empty string! We must be at the root. */

    strcpy ( fname, SLASH ) ;
    return ;
  }

  /* Otherwise, just delete the last element of the path. */

  /* Remove the trailing slash - then remove the rest as
     if it was a file name */

  fname [ strlen(fname)-1 ] = '\0' ;
  chop_file ( fname ) ;
}


void puFileSelector::input_entered ( puObject* input )
{
  puFileSelector* file_selector = (puFileSelector*) (input -> getUserData ()) ;

  /*
    If the input selector is empty - do nothing
  */

  if ( input -> getStringValue() [ 0 ] == '\0' )
    return ;

  if ( ulIsAbsolutePathName ( input -> getStringValue () ) )
    strcpy ( file_selector->getStringValue (), input ->getStringValue () ) ;
  else
  if ( strlen ( file_selector->__getStartDir() ) +
       strlen ( input->getStringValue() ) + 2 >= PUSTRING_MAX )
  {
    ulSetError ( UL_WARNING,
      "PUI:puFileSelector - path name is too long");
    strcpy ( file_selector->getStringValue (), file_selector->__getStartDir());
  }
  else
  {
    //strcpy ( file_selector->getStringValue (), file_selector->__getStartDir());
    //strcat ( file_selector->getStringValue (), SLASH    ) ;
    //strcat ( file_selector->getStringValue (), input -> getStringValue () ) ;

    ulMakePath ( file_selector->getStringValue (),
      file_selector->__getStartDir(), input -> getStringValue () ) ;
  }

  /*
    If he typed a valid directory name - then
    refresh the contents of the browser window
  */

  ulDir *d = ulOpenDir ( file_selector->getStringValue() ) ;
  ulCloseDir ( d ) ;

  if ( d != NULL )
    file_selector -> find_files () ;
}


void puFileSelector::handle_select ( puObject* list_box )
{
  puFileSelector* file_selector = (puFileSelector*) list_box -> getUserData () ;

  int selected ;
  list_box -> getValue ( &selected ) ;

  if ( selected >= 0 && selected < file_selector -> num_files )
  {
    char *dst = file_selector -> getStringValue () ;
    char *src = file_selector -> files [ selected ] ;

    chop_file ( dst ) ;

    if ( strcmp ( src, "[..]" ) == 0 )
    {
      /* Do back up one level - so refresh. */

      go_up_one_directory ( dst ) ;
      file_selector -> find_files () ;
      file_selector -> __getInput() -> setValue ( dst ) ;
      return ;
    } 

    if ( file_selector -> dflag [ selected ] )
    {
      /* If this is a directory - then descend into it and refresh */

      if ( strlen ( dst ) + strlen ( src ) + 2 >= PUSTRING_MAX )
      {
	ulSetError ( UL_WARNING,
          "PUI: puFileSelector - path is too long, max is %d.", PUSTRING_MAX ) ;
	return ;
      }

      strcat ( dst, &src[1] ) ;  /* Remove leading '[' */
      dst [ strlen ( dst ) - 1 ] = SLASH[0] ;  /* Replace trailing ']' with slash */
      file_selector -> find_files () ;
      file_selector -> __getInput() -> setValue ( dst ) ;
      return ;
    }

    /* If this is a regular file - then just append it to the string */

    if ( strlen ( dst ) + strlen ( src ) + 2 >= PUSTRING_MAX )
    {
      ulSetError ( UL_WARNING, 
        "PUI: puFileSelector - path is too long, max is %d.", PUSTRING_MAX ) ;
      return ;
    }

    strcat ( dst, src ) ;
    file_selector -> __getInput() -> setValue ( dst ) ;
  }
  else
  {
    /*
      The user clicked on blank screen - maybe we should
      refresh just in case some other process created the
      file.
    */

    file_selector -> find_files () ;
  }
}


static void puFileSelectorHandleCancel ( puObject* b )
{
  puFileSelector* file_selector = (puFileSelector*) b -> getUserData () ;
  file_selector -> setValue ( "" ) ;
  file_selector -> invokeCallback () ;
}

static void puFileSelectorHandleOk ( puObject* b )
{
  puFileSelector* file_selector = (puFileSelector*) b -> getUserData () ;

  file_selector -> invokeCallback () ;
}

void puFileSelector::setSize ( int w, int h )
{
  // Resize the frame widget
  frame->setSize ( w, h ) ;

  // Resize and position the slider
  slider->setPosition ( w-30, 40+20*arrow_count ) ;
  slider->setSize ( 20, h-70-40*arrow_count ) ;

  // Position the arrow buttons
  if ( up_arrow )
  {
    up_arrow->setPosition ( w-30, h-30-20*arrow_count ) ;
    down_arrow->setPosition ( w-30, 20+20*arrow_count ) ;
  }

  if ( fastup_arrow )
  {
    fastup_arrow->setPosition ( w-30, h-50 ) ;
    fastdown_arrow->setPosition ( w-30, 40 ) ;
  }

  // Resize the list box
  list_box->setSize ( w-40, h-70 ) ;

  // Resoze and position the buttons
  cancel_button->setSize ( (w<170)?(w/2-15):70, 20 ) ;
  ok_button->setSize ( (w<170)?(w/2-15):70, 20 ) ;
  ok_button->setPosition ( (w<170)?(w/2+5):90, 10 ) ;
}

puFileSelector::puFileSelector ( int x, int y, int w, int h, int arrows, const char* dir, const char *title )
                           : puDialogBox ( x, y )
{
  puFileSelectorInit ( x, y, w, h, arrows, dir, title ) ;
}

puFileSelector::puFileSelector ( int x, int y, int w, int h, const char* dir, const char *title )
                           : puDialogBox ( x, y )
{
  puFileSelectorInit ( x, y, w, h, 1, dir, title ) ;
}

puFileSelector::puFileSelector ( int x, int y, int arrows, const char* dir, const char *title )
                           : puDialogBox ( x, y )
{
  puFileSelectorInit ( x, y, arrows, 220, 170, dir, title ) ;
}

puFileSelector::puFileSelector ( int x, int y, const char* dir, const char *title )
                           : puDialogBox ( x, y )
{
  puFileSelectorInit ( x, y, 220, 170, 1, dir, title ) ;
}

puFileSelector::~puFileSelector ()
{
  if ( files )
  {
    for ( int i=0; i<num_files; i++ )
      delete files[i];

    delete[] files;
    delete[] dflag;
  }

  if ( this == puActiveWidget () )
    puDeactivateWidget () ;
}

void puFileSelector::puFileSelectorInit ( int x, int y, int w, int h, 
                                          int arrows,
                                          const char *dir,
                                          const char *title )
{
  type |= PUCLASS_FILESELECTOR ;

  files = NULL ;
  dflag = NULL ;
  num_files = 0 ;

  if ( ulIsAbsolutePathName ( dir ) )
    strcpy ( startDir, dir ) ;
  else
  {
    if ( ulGetCWD ( startDir, PUSTRING_MAX ) == NULL ||
       strlen ( startDir ) + 1 + strlen ( dir ) + 1 > PUSTRING_MAX )
    {
      ulSetError ( UL_WARNING,
      "PUI:puFileSelector - can't find current directory or name is too long");
      setValue ( "" ) ;
      invokeCallback () ;
      return ;
    }

    if ( ! is_slash ( startDir[strlen(startDir)-1] ) )
      strcat ( startDir, SLASH ) ;

    strcat ( startDir, dir ) ;
  }

  if ( ! is_slash ( startDir[strlen(startDir)-1] ) )
    strcat ( startDir, SLASH ) ;

  strcpy ( getStringValue (), startDir ) ;

  if ( arrows > 2 ) arrows = 2 ;
  if ( arrows < 0 ) arrows = 0 ;
  arrow_count = arrows ;

  frame = new puFrame ( 0, 0, w, h );

  slider = new puSlider (w-30,40+20*arrows,h-70-40*arrows,TRUE,20);
  slider->setDelta(0.1f);
  slider->setValue(1.0f);
  slider->setSliderFraction (0.2f) ;
  slider->setCBMode( PUSLIDER_DELTA );
  
  list_box = new puListBox ( 10, 60, w-40, h-30 ) ;
  list_box -> setLabel ( title );
  list_box -> setLabelPlace ( PUPLACE_ABOVE ) ;
  list_box -> setStyle ( -PUSTYLE_SMALL_SHADED ) ;
  list_box -> setUserData ( this ) ;
  list_box -> setCallback ( handle_select ) ;
  list_box -> setValue ( 0 ) ;

  find_files () ;

  handle_select ( list_box ) ;

  slider -> setUserData ( list_box ) ;
  slider -> setCallback ( puFileSelectorHandleSlider ) ;

  input = new puInput ( 10, 40, w-40, 60 ) ;
  input -> setValue ( startDir ) ;
  input -> setUserData ( this ) ;
  input -> setCallback ( input_entered ) ;

  cancel_button = new puOneShot ( 10, 10, (w<170)?(w/2-5):80, 30 ) ;
  cancel_button -> setLegend ( "Cancel" ) ;
  cancel_button -> setUserData ( this ) ;
  cancel_button -> setCallback ( puFileSelectorHandleCancel ) ;

  ok_button = new puOneShot ( (w<170)?(w/2+5):90, 10, (w<170)?(w-10):160, 30 ) ;
  ok_button -> setLegend ( "Ok" ) ;
  ok_button -> setUserData ( this ) ;
  ok_button -> setCallback ( puFileSelectorHandleOk ) ;

  up_arrow = (puArrowButton *)NULL ;  down_arrow = (puArrowButton *)NULL ;
  fastup_arrow = (puArrowButton *)NULL ;  fastdown_arrow = (puArrowButton *)NULL ;

  if ( arrows > 0 )
  {
    down_arrow = new puArrowButton ( w-30, 20+20*arrows, w-10, 40+20*arrows, PUARROW_DOWN ) ;
    down_arrow->setUserData ( slider ) ;
    down_arrow->setCallback ( puFileSelectorHandleArrow ) ;

    up_arrow = new puArrowButton ( w-30, h-30-20*arrows, w-10, h-10-20*arrows, PUARROW_UP ) ;
    up_arrow->setUserData ( slider ) ;
    up_arrow->setCallback ( puFileSelectorHandleArrow ) ;
  }

  if ( arrows == 2 )
  {
    fastdown_arrow = new puArrowButton ( w-30, 40, w-10, 60, PUARROW_FASTDOWN ) ;
    fastdown_arrow->setUserData ( slider ) ;
    fastdown_arrow->setCallback ( puFileSelectorHandleArrow ) ;

    fastup_arrow = new puArrowButton ( w-30, h-50, w-10, h-30, PUARROW_FASTUP ) ;
    fastup_arrow->setUserData ( slider ) ;
    fastup_arrow->setCallback ( puFileSelectorHandleArrow ) ;
  }

  close  () ;
  reveal () ;
}

static int puFileSelectorStringCompare ( const char *s1, const char *s2,
                                       const char  f1, const char  f2 )
{
  if ( f1 > f2 )    /* Directories before regular files. */
    return -1 ;

  if ( f1 < f2 )
    return 1 ;

  while ( 1 )
  {
    char c1 = s1? (*s1++): 0 ;
    char c2 = s2? (*s2++): 0 ;
    
    //end of string?
    if ( !c1 || !c2 )
    {
      if ( c1 )
        return 1 ; //s1 is longer
      if ( c2 )
        return -1 ; //s1 is shorter
      return 0 ;
    }
    
    if ( c1 == c2 )
      continue ;

    /*
      Windoze users are case-insensitive - so they presumably
      want case ignored in the sorting.

      UNIX users, however, do things like calling their makefiles
      'Makefile' so as to force them to the start of the directory
      listing.
    */

#if defined(WIN32) && !defined(__CYGWIN__)
    if ( c1 >= 'a' && c1 <= 'z' )
      c1 = c1 - ('a'-'A') ;
    
    if ( c2 >= 'a' && c2 <= 'z' )
      c2 = c2 - ('a'-'A') ;
#endif
    
    if ( c1 != c2 )
    {
      if ( c1 < c2 )
        return -1 ;
      return 1 ;
    }
  }
  return 0 ;
}


void puFileSelector::setInitialValue ( char *s )
{
  if ( ulIsAbsolutePathName ( s ) )
  {
    input -> setValue ( s ) ;
    input -> invokeCallback () ;
  }
  else
  {
    strcat ( input -> getStringValue (), s ) ;
    input -> invokeCallback () ;
  }
}


static void puFileSelectorSort ( char** list, char *flags, int size )
//
//  comb sort - a modified bubble sort
//    taken from BYTE, April 1991, ppg 315-320
//
{
  int switches ;
  int gap = size ;

  do
  {
    gap = ((gap * 197) >> 8);  // gap /= 1.3;

    switch (gap)
    {
      case 0:  // the smallest gap is 1 -- bubble sort
	gap = 1;
	break;

      case 9:  // this is what makes this Combsort11
      case 10:
	gap = 11;
	break;
    }

    switches = 0 ; // dirty pass flag

    int top = size - gap;

    for ( int i=0; i<top; ++i )
    {
      int j=i+gap;

      if ( puFileSelectorStringCompare ( list [i], list [j],
                                       flags[i], flags[j] ) > 0 )
      {
        char* temp = list[i] ;
        list[i] = list[j] ;
        list[j] = temp ;

        char tmp2 = flags[i] ;
        flags[i] = flags[j] ;
        flags[j] = tmp2 ;

        ++switches;
      }
    }
  } while ( switches || gap > 1 ) ;
}


void puFileSelector::find_files ()
{
  if ( files != NULL )
  {
    for ( int i = 0 ; i < num_files ; i++ )
      delete files[i] ;

    delete [] files ;
    delete [] dflag ;
  }

  num_files = 0 ;

  char dir [ PUSTRING_MAX * 2 ] ;

  strcpy ( dir, getStringValue() ) ;

  if ( ! is_slash ( dir [ strlen ( dir ) - 1 ] ) )  /* Someone forgot a trailing '/' */
    strcat ( dir, SLASH ) ;

  int ifile = 0 ;

  ulDir    *dirp = ulOpenDir ( dir ) ;
  ulDirEnt *dp ;

  if ( dirp == NULL )
  {
    perror ("puFileSelector") ;
    ulSetError ( UL_WARNING, "PUI:puFileSelector - can't open directory '%s'", dir ) ;
    num_files = 0 ;
  }

  /* Count the number of files - skipping "." */

  while ( ( dp = ulReadDir(dirp) ) != NULL )
    if ( strcmp ( dp->d_name, "." ) != 0 )
      ifile++ ;

  ulCloseDir ( dirp ) ;

  num_files = ifile ;

  if ( num_files == 0 )
  {
    ulSetError ( UL_WARNING,
		   "PUI:puFileSelector - no entries in directory '%s'?!", dir ) ;
    num_files = 0 ;
    return;
  }

  files = new char* [ num_files+1 ] ;
  dflag = new char  [ num_files+1 ] ;

  dirp = ulOpenDir ( dir ) ;

  if ( dirp == NULL )
  {
    perror ("puFileSelector") ;
    ulSetError ( UL_WARNING,
                   "PUI:puFileSelector - can't re-open directory '%s'", dir ) ;
    num_files = 0 ;
    return ;
  }

  for ( ifile = 0 ; (dp = ulReadDir(dirp)) != NULL && ifile < num_files ; )
  {
    /* Skip over the "." entry... */

    if ( strcmp ( dp->d_name, "." ) == 0 )
      continue ;

    dflag[ ifile ] = dp->d_isdir ;

    if ( dflag[ ifile ] )
    {
      files[ ifile ] = new char[ strlen(dp->d_name)+4 ] ;
      strcpy ( files [ ifile ], "[" ) ;
      strcat ( files [ ifile ], dp->d_name ) ;
      strcat ( files [ ifile ], "]" ) ;
    }
    else
    {
      files[ ifile ] = new char[ strlen(dp->d_name)+1 ] ;
      strcpy ( files [ ifile ], dp->d_name ) ;
    }

    ifile++ ;
  }

  files [ ifile ] = NULL ;

  ulCloseDir ( dirp ) ;

  puFileSelectorSort( files, dflag, num_files ) ;

  list_box -> newList ( files ) ;
}

void puFileSelector::setStyle( int style )
{
  frame->setStyle ( style ) ;
  list_box->setStyle ( style ) ;
  slider->setStyle ( style ) ;
  cancel_button->setStyle ( style ) ;
  ok_button->setStyle ( style ) ;
  input->setStyle ( style ) ;

  if ( down_arrow != NULL )
  {
    down_arrow->setStyle ( style ) ;
    up_arrow->setStyle ( style ) ;
    if ( fastdown_arrow != NULL )
    {
      fastdown_arrow->setStyle ( style ) ;
      fastup_arrow->setStyle ( style ) ;
    }
  }

  puObject::setStyle ( style ) ;
}

void puFileSelector::setColour( int which, float r, float g, float b, float  a )
{
  frame->setColour ( which, r, g, b, a ) ;
  list_box->setColour ( which, r, g, b, a ) ;
  slider->setColour ( which, r, g, b, a ) ;
  cancel_button->setColour ( which, r, g, b, a ) ;
  ok_button->setColour ( which, r, g, b, a ) ;
  input->setColour ( which, r, g, b, a ) ;

  if ( down_arrow != NULL )
  {
    down_arrow->setColour ( which, r, g, b, a ) ;
    up_arrow->setColour ( which, r, g, b, a ) ;
    if ( fastdown_arrow != NULL )
    {
      fastdown_arrow->setColour ( which, r, g, b, a ) ;
      fastup_arrow->setColour ( which, r, g, b, a ) ;
    }
  }

  puObject::setColour ( which, r, g, b, a ) ;
}

void puFileSelector::setBorderThickness( int t )
{
  frame->setBorderThickness ( t ) ;
  list_box->setBorderThickness ( t ) ;
  slider->setBorderThickness ( t ) ;
  cancel_button->setBorderThickness ( t ) ;
  ok_button->setBorderThickness ( t ) ;
  input->setBorderThickness ( t ) ;

  if ( down_arrow != NULL )
  {
    down_arrow->setBorderThickness ( t ) ;
    up_arrow->setBorderThickness ( t ) ;
    if ( fastdown_arrow != NULL )
    {
      fastdown_arrow->setBorderThickness ( t ) ;
      fastup_arrow->setBorderThickness ( t ) ;
    }
  }

  puObject::setBorderThickness ( t ) ;
}




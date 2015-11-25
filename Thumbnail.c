/*	File:		Thumbnail.c	Contains:	Sample to demonstrate using the Picture Utilities Package					The guts of this is in the window creation function: DoCreateWindow( PicHandle thePicture )					DoCreateWindow is passed a picture handle and if the global gUsePictPalette is set it				will use the pict utils package (PUP) to get the color table stored in the pict.  You could 				this source to sample direct color images, but to keep things as clear as possible this				application only deals with 8 bit images.					We use the PUP to get the color table for the image.  This is passed into NewGWorld and it				is also used to create a palette that is attached to the window.					Note that we use PUP to sample the image that was stored in the file.  We interrogate a				PicHandle read directly from the file.  There are also routines for sampling PixMaps,				but dont forget that if you are in an indexed color environment, and you sample a PixMap, 				if you didnt set up the color environment when your first created the PixMap, the colors				may not be as you expect.  								To see an example of this there are two 8-bit images included with this snippet.				One is a spectral ramp, the other a green/brow ramp.  Both have approx 120 or so colors.				to see an example of what PUP may report if you didnt set the colors for a window up				properly before displaying open one of the files with the "use picture palette" item 				from the palette menu toggled off.					in this case it will use the "wrong" set of colors to try to draw the image (it just uses				what ever the default system color table is).  If you get PUP to sample (by toggling on				the "use picture palette" item from the palette menu the picture is rendered correctly.	Written by: NT		Copyright:	Copyright � 1994-1999 by Apple Computer, Inc., All Rights Reserved.				You may incorporate this Apple sample source code into your program(s) without				restriction. This Apple sample source code has been provided "AS IS" and the				responsibility for its operation is yours. You are not permitted to redistribute				this Apple sample source code as "Apple sample source code" after having made				changes. If you're going to re-distribute the source, we require that you make				it clear in the source that the code was descended from Apple sample source				code, but that you've made changes.	Change History (most recent first):				08/2000		JM				Carbonized, non-Carbon code is commented out											for demonstration purposes.				7/14/1999	KG				Updated for Metrowerks Codewarror Pro 2.1				*//* Constant Declarations */#include "CarbonPrefix.h"#include <menus.h>#include <PictUtils.h>#include <QDOffScreen.h>#include <Fonts.h>#include <Windows.h>#include <Menus.h>#include <TextEdit.h>#include <Dialogs.h>#include <DiskInit.h>//#include <StandardFile.h>#include <Navigation.h>#include <Devices.h>#define	WWIDTH		470#define	WHEIGHT		330//#define WLEFT		(((screenBits.bounds.right - screenBits.bounds.left) - WWIDTH) / 2)//#define WTOP		(((screenBits.bounds.bottom - screenBits.bounds.top) - WHEIGHT) / 2)#define HiWrd(aLong)	(((aLong) >> 16) & 0xFFFF)#define LoWrd(aLong)	((aLong) & 0xFFFF)enum {	mApple = 128,	mFile,	mPalette,	mTest} ;enum {	iAbout = 1} ;enum {	iOpen = 1,	iClose,	iUnused1,	iQuit = 4} ;enum {	iUsePictPalette = 1} ;static Boolean gQuitFlag = false ; Point gStaggerPos = {50,50} ;static Boolean gUsePictPalette = false ;// function prototypesvoid InitToolbox( void ) ;void MainEventLoop( void ) ;void HandleKeyPress(EventRecord *event) ;void HandleOSEvent(EventRecord *event) ;void HandleMenuCommand(long menuResult) ;PicHandle DoReadPICT( short theRef, OSErr *theErr ) ;OSErr DoCreateWindow( PicHandle thePicture ) ;void AdjustMenus( void ) ;const RGBColor	kRGBBlack = {0, 0, 0};const RGBColor	kRGBWhite = {0xFFFF, 0xFFFF, 0xFFFF};void main(){	InitToolbox() ;		MainEventLoop();}void InitToolbox(){	Handle		menuBar = nil;	// Most InitSomething calls are automatic in carbon	// Only InitCursor is still called when initializing the toolbox	//InitGraf((Ptr) &qd.thePort);	//InitFonts();	//InitWindows();	//InitMenus();	//TEInit();	//InitDialogs(0L);	InitCursor();	// initialize application globals		gQuitFlag = false;			menuBar = GetNewMBar(128);				// Read menus into menu bar, MBAR res id is 128		if ( menuBar == nil )		 ExitToShell();						// if we dont have it then quit - your app 		 									// needs a dialog here 	SetMenuBar(menuBar);					// Install menus	DisposeHandle(menuBar);		//AppendResMenu(GetMenuHandle(mApple), 'DRVR');	// Add DA names to Apple menu, ID 128	DrawMenuBar();}void MainEventLoop(){	EventRecord 	event;	WindowPtr   	window;	short       	thePart;	Rect        	screenRect, tempRect;	Point			aPoint = {100, 100};	GWorldPtr		theNewWorld ;	PixMapHandle	offPixMap ;	GrafPtr			oldPort ;	while( !gQuitFlag )	{		if (WaitNextEvent( everyEvent, &event, 0, nil ))		{			AdjustMenus() ;			switch (event.what) {				case mouseDown:									thePart = FindWindow( event.where, &window );										switch( thePart ) {						case inMenuBar: 							HandleMenuCommand(MenuSelect(event.where));							break;												case inDrag:												//screenRect = (**GetGrayRgn()).rgnBBox;							GetRegionBounds ( GetGrayRgn(), &screenRect );							DragWindow( window, event.where, &screenRect );							break ;											case inContent:												if (window != FrontWindow())								SelectWindow( window );							break ;											case inGoAway:							if (TrackGoAway( window, event.where )) {								DisposeWindow ( window );							}							break ;													default:							break ;					}					break ;																	case updateEvt:									window = (WindowPtr)event.message;					GetPort(&oldPort ) ;						//SetPort( window );					SetPortWindowPort ( window );										BeginUpdate( window );										// get the GWorld from the window refcon					theNewWorld = (GWorldPtr)GetWRefCon ( window );					offPixMap = GetGWorldPixMap( theNewWorld ) ;					(void) LockPixels( offPixMap ) ;					/*CopyBits( &((GrafPtr)theNewWorld)->portBits,							  &window->portBits,							  &window->portRect,							  &window->portRect,							  srcCopy,							  nil ) ;*/					CopyBits( 	GetPortBitMapForCopyBits(theNewWorld),								GetPortBitMapForCopyBits(GetWindowPort(window)),								GetPortBounds(GetWindowPort(window), &tempRect),								GetPortBounds(GetWindowPort(window), &tempRect),								srcCopy,								nil );					(void) UnlockPixels( offPixMap ) ;					EndUpdate( window );					SetPort( oldPort ) ;					break ;									case keyDown:				case autoKey:					HandleKeyPress(&event);					break;									case diskEvt:					/*if ( HiWrd(event.message) != noErr ) 						(void) DIBadMount(aPoint, event.message);*/					break;									case osEvt:				case activateEvt:					break;			}		}	}}void HandleKeyPress(EventRecord *event){	char	key;	key = event->message & charCodeMask;		// just check to see if we want to quit...		if ( event->modifiers & cmdKey ) {		/* Command key down? */		HandleMenuCommand(MenuKey(key));	} }void HandleMenuCommand(long menuResult){	short		menuID;	short		menuItem;	//Str255		daName;	// not needed after carbonization	DialogPtr	theDialog ; 	short		itemHit ;	//SFTypeList	myTypes = { 'PICT' } ;	PicHandle	thePicture ;	OSErr		err = noErr;	short		theRef ;	// Variables added for carbon    NavDialogOptions    dialogOptions;	NavReplyRecord		reply;    AEDesc              defaultLocation;	AEKeyword   		theKeyword;    DescType    		actualType;    Size        		actualSize;    FSSpec      		documentFSSpec;		//StandardFileReply	theSFReply ;	menuID = HiWrd(menuResult);	menuItem = LoWrd(menuResult);	switch ( menuID ) {		case mApple:			switch ( menuItem ) {				case iAbout:					theDialog = GetNewDialog ( 128, nil, (WindowPtr)-1 );					do {						ModalDialog ( nil, &itemHit );					} while( itemHit != ok ) ;					DisposeDialog ( theDialog );					break;									default:					// Desk Accessories not supported in carbon					// They are automatically handled					//GetMenuItemText(GetMenuHandle(mApple), menuItem, daName);					//(void) OpenDeskAcc(daName);					break;			}			break;		case mFile:			switch ( menuItem ) {				case iOpen:					/*					// Get the file name to open					StandardGetFile( nil, 1, myTypes, &theSFReply ) ;										// did the user cancel?					if(!theSFReply.sfGood)						break ;										// open the file					err = FSpOpenDF( &theSFReply.sfFile, fsRdPerm, &theRef ) ;										if( err != noErr )						break ; 	// should handle this properly											thePicture = DoReadPICT( theRef, &err ) ;										if( err != noErr )						break ; 	// should handle this properly									// display the contents					err = DoCreateWindow( thePicture ) ;					*/	/*		The Standard File Package is no longer supported in Carbon.		Navigation services is to be used instead.  Above this comment		is the pre-carbon way of opening a dialog and selecting a file,		below is the carbon way.	*/					/*OSErr NavGetFile (  									 	AEDesc *defaultLocation,    										NavReplyRecord *reply,     									NavDialogOptions *dialogOptions,     									NavEventUPP eventProc,     									OSType fileType,     									OSType fileCreator,     									void *callBackUD );*/					// Create a "NavDialogOptions" and use defaults, but set					// the directory to the app's directory					NavGetDefaultDialogOptions(&dialogOptions);					dialogOptions.dialogOptionFlags |= kNavSelectDefaultLocation;					AECreateDesc(typeFSS, &documentFSSpec,                             sizeof(documentFSSpec),                             &defaultLocation );					err = NavGetFile (&defaultLocation, &reply, &dialogOptions,                                nil, nil, nil,                                nil, nil);										if (err != noErr || !reply.validRecord)						break;					// Get a pointer to selected file                    err = AEGetNthPtr(&(reply.selection), 1,                                            typeFSS, &theKeyword,                                            &actualType,&documentFSSpec,                                            sizeof(documentFSSpec),                                            &actualSize);					if (err != noErr)						break;										// open the file					err = FSpOpenDF( &documentFSSpec, fsRdPerm, &theRef ) ;										if( err != noErr )						break ; 	// should handle this properly											thePicture = DoReadPICT( theRef, &err ) ;										if( err != noErr )						break ; 	// should handle this properly									// display the contents					err = DoCreateWindow( thePicture ) ;					NavDisposeReply(&reply);					AEDisposeDesc(&defaultLocation);										break ;									case iClose:					DisposeWindow ( FrontWindow() );					break ;				case iQuit:					gQuitFlag = true;					break;			}			break;					case mPalette:			switch ( menuItem ) {				case iUsePictPalette :					// toggle the check mark and the global boolean					gUsePictPalette = !gUsePictPalette ;					// For menus, use ActionMenuItem as opposed to ActionItem					//CheckItem ( GetMenuHandle ( mPalette ), iUsePictPalette, gUsePictPalette );					CheckMenuItem ( GetMenuHandle (mPalette), iUsePictPalette, gUsePictPalette );					break ;			}			break ;					case mTest:		//--------------------------- T E S T   R O U T I N E ---------------------------------			if( menuItem == 1 ) {				// get some of the screen				void ShrinkToBWPict( WindowPtr theWindow ) ;		// prototype								// because I have multiple monitors and I wanted to test on them				// I set up the area below, you may want something rather different				// YOU WILL PROBABLY NEED TO CHANGE THIS								Rect area = { 0, -50, 200, 200 } ;				// area to save 				ShrinkToBWPict( FrontWindow() ) ;								//--------------------------- T E S T   R O U T I N E ---------------------------------			}			break; 	}	HiliteMenu(0);		// Unhighlight whatever MenuSelect or MenuKey hilited}void AdjustMenus( void ) {	WindowPtr	theWindow ;	theWindow = FrontWindow() ;	if( theWindow != nil ) {		// In carbon use ActionMenuItem instead of ActionItem		//EnableItem ( GetMenuHandle ( mFile ), iClose );		//EnableItem ( GetMenuHandle ( mTest ), 0 );	// <== 0 is entire menu		EnableMenuItem ( GetMenuHandle ( mFile ), iClose );		EnableMenuItem ( GetMenuHandle ( mTest ), 0 );	// <== 0 is entire menu	}	else {		// In carbon use ActionMenuItem instead of ActionItem		//DisableItem ( GetMenuHandle ( mFile ), iClose );		//DisableItem ( GetMenuHandle ( mTest ), 0 );	// <== 0 is entire menu		DisableMenuItem ( GetMenuHandle ( mFile ), iClose );		DisableMenuItem ( GetMenuHandle ( mTest ), 0 );	// <== 0 is entire menu	}	// make sure the check marks are correct	// Again, use ActionMenuItem instead of ActionItem	//CheckItem ( GetMenuHandle ( mPalette ), iUsePictPalette, gUsePictPalette );	CheckMenuItem ( GetMenuHandle ( mPalette ), iUsePictPalette, gUsePictPalette );}PicHandle DoReadPICT( short theRef, OSErr *theErr ) {	long		theFileSize ;	PicHandle	thePicture ;		// pict files have a 512 byte header at the front - we dont care about this	// we can find the size of the pict by subtracting 512 bytes from the length	// of the file.  We then want to resize the handle to that and read the data	// into the resized handle.		if(( *theErr = GetEOF( theRef, &theFileSize )) != noErr ) {		FSClose( theRef ) ;		return nil ; 	}		if(( *theErr = SetFPos( theRef, fsFromStart, 512)) != noErr ) {		FSClose( theRef ) ;		return nil ; 	}	theFileSize -= 512 ;		thePicture = (PicHandle)NewHandle( theFileSize ) ;	if( thePicture == nil ) {		FSClose( theRef ) ;		*theErr = MemError() ;		return nil ; 		// what ever the mem manager error was	}		HLock( (Handle)thePicture ) ;	*theErr = FSRead( theRef, &theFileSize, (Ptr)*thePicture ) ;	HUnlock(  (Handle)thePicture ) ;		if( *theErr != noErr ) {		FSClose( theRef ) ;		return nil ; 	}	return thePicture ;	}OSErr DoCreateWindow( PicHandle thePicture ){	Rect		theRect ;	OSErr		theErr ;	GWorldPtr	theNewWorld ;	CGrafPtr	savedPort ;	WindowPtr	theWindow ;	GDHandle	oldDevice ;		PictInfo		thePictInfo ;	PaletteHandle	thePictPalette = nil ;	CTabHandle		thePictCTab = nil ;		// make an offscreen environment and image the pict into this	// Make a window the size of the pict	// store a reference to the GWorld in the Refcon of the window	// invalidate the window content area.		theRect = (**thePicture).picFrame;		// to we want to attempt to sample the picture... 	if( gUsePictPalette ) {			// use the picture utilities to get the palette for the window		theErr = GetPictInfo( thePicture, &thePictInfo, returnColorTable, 256, systemMethod, 0) ;				// set up the palette and color table for later use		thePictPalette = NewPalette( 256, thePictInfo.theColorTable, pmTolerant, 0x1000 ) ;		thePictCTab = thePictInfo.theColorTable ;	}		// we are only dealing with eight bit images here	// hence the requirement for the picture utils package to 	// return 256 colors, and we will create a gWorld 8 bits deep		theErr = NewGWorld( &theNewWorld, 8, &theRect, thePictCTab, nil, 0L ) ;		if( theErr != noErr ) 		return theErr ;		// save the world	GetGWorld( &savedPort, &oldDevice ) ;	SetGWorld( theNewWorld, nil ) ;			RGBForeColor( &kRGBBlack ) ;		// ensure the fg and bg colors are 	RGBBackColor( &kRGBWhite ) ;		// as anticipated	EraseRect( &theRect ) ;				// clear the area for the pict	PenMode( srcCopy ) ;				// ensure the t/f mode is as expected	SetOrigin( theRect.left, theRect.top );		// render the image into the offscreen buffer	DrawPicture( thePicture, &theRect ) ;	SetOrigin( 0, 0 );		SetGWorld( savedPort, oldDevice ) ;		// create the window	OffsetRect( &theRect, gStaggerPos.h, gStaggerPos.v) ;	gStaggerPos.h += 16 ;	gStaggerPos.v += 16 ;		// heh - should roll these around, but you wont 								// create more than a couple of windows, will you  :-)								 	theWindow  = NewCWindow( nil, &theRect, "\pplayTime", true, 								documentProc, (WindowPtr)-1, true, (long)theNewWorld );						// and if we set up the palette earlier assign it to the window					if( thePictPalette != nil ) {		SetPalette ( theWindow, thePictPalette, true );	}		ActivatePalette ( theWindow );			// make sure it is visible	ShowWindow( theWindow ) ;		SetGWorld( (CGrafPtr)theWindow, nil ) ;		// invalidate the content region of the window - we don't do any drawing to it here.	// Not supported in carbon	//InvalRect ( &theRect );	InvalWindowRect ( theWindow, &theRect );		SetGWorld( savedPort, oldDevice ) ;	return noErr;}
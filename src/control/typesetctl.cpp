/*!
        Copyright (c) 2006-2008, Reinhard Katzmann, Matevž Jekovec, Canorus development team
        All Rights Reserved. See AUTHORS for a complete list of authors.
        
        Licensed under the GNU GENERAL PUBLIC LICENSE. See COPYING for details.
*/

// Includes
#include <QTemporaryFile>

#include "export/export.h"
#include "control/externprogram.h"
#include "control/typesetctl.h"
//#include "core/document.h"

/*!	\class CATypesetCtl
	\brief Interface to start a typesetter in the background

	This class is used to run a typesetter in the background while receiving
	the output of it. The output of the typesetter can be fetched via signal/slots.
	If the typesetter does not support creation of pdf files another process can
	be started to do the conversion.

	Constructor:
*/

CATypesetCtl::CATypesetCtl()
{
	_poTypesetter = new CAExternProgram;
	_poConvPS2PDF = new CAExternProgram;
	_poExport = new CAExport;
	_bPDFConversion = false;
	connect( _poTypesetter, SIGNAL( programExited( int ) ), this, SLOT( typsetterFinished( int ) ) );
}

// Destructor
CATypesetCtl::~CATypesetCtl()
{
	if( _poTypesetter )
		delete _poTypesetter;
	_poTypesetter = 0;
	if( _poConvPS2PDF )
		delete _poConvPS2PDF;
	_poConvPS2PDF = 0;
	if( _poExport )
		delete _poExport;
	_poExport = 0;
}

/*!
	Defines the typesetter executable name to be run

	This method let's you define the typesetter
	executable name (with optionally included path name)
	

	\sa setTSetOption( QVariant oName, QVariant oValue );
	\sa CAExternProgram::execProgram( QString oCwd )
*/
void CATypesetCtl::setTypesetter( const QString &roProgramName, const QString &roProgramPath )
{
	if( !roProgramName.isEmpty() )
	{
		_poTypesetter->setProgramName( roProgramName );
		if( !roProgramPath.isEmpty() )
			_poTypesetter->setProgramPath( roProgramPath );
	}
}

/*!
	Defines the postscript->pdf executable name to be run

	This method let's you define the executble name of an optional
	postscript->pdf converter in case the typesetter does not
	support the output of pdf. (with optionally included path name
	and a list of parameters)	

	\sa createPDF();
	\sa CAExternProgram::execProgram( QString oCwd )
*/
void CATypesetCtl::setPS2PDF( const QString &roProgramName, const QString &roProgramPath, const QStringList &roParams )
{
	if( !roProgramName.isEmpty() && !roProgramPath.isEmpty() )
	{
		_poConvPS2PDF->setProgramName( roProgramName );
		if( !roProgramPath.isEmpty() )
			_poConvPS2PDF->setProgramPath( roProgramPath );
		if( !roParams.isEmpty() )
			_poConvPS2PDF->setParameters( roParams );
	}
}

/*!
	Defines the export options

	For the run of one exporter additional options can be defined using
	this method. The \a oName does define the name of the option
	and the \a oValue defines the value of the option name to be
	passed to the exporter. There is no conversion / transformation
	done in the base class.

	\sa exportDocument();
*/
void CATypesetCtl::setExpOption( const QVariant &roName, const QVariant &roValue )
{
	_oExpOptList.append( roName );
	_oExpOptList.append( roValue );
}

/*!
	Defines the typesetter options

	For the run of one typesetter additional parameters can be defined using
	this method. The \a oName does define the name of the parameter
	and the \a oValue defines the value of the parameter name to be
	passed to the typesetter.

	The name and values are converted to a string (so only QVariants that can
	be converted to QString are supported) in the form "-<name>=<option>"
	(without < and > signs) with no additional apostrophes.

	\sa runTypesetter();
*/
void CATypesetCtl::setTSetOption( const QVariant &roName, const QVariant &roValue )
{
	_oTSetOptList.append( roName );
	_oTSetOptList.append( roValue );
	// Subclass needs to transform the stored options to program parameters and
  // call _poTypesetter->setParameters( oTPSParameters )
	// Primitive solution is here: Convert Name and Value to string and store it to string list
	if( !roName.toString().isEmpty() && !roValue.toString().isEmpty() )
	{
		_poTypesetter->addParameter( QString("-")+roName.toString()+"="+roValue.toString() );
	}
	else
	  qWarning("TypesetCtl: Ignoring typesetter option name being empty!");
}

/*!
	Export the file to disk to be run by the typesetter

	This method creates a random file name as a stream file name
	for the exporter. Then it exports the document \a poDoc.

	\sa setExpOption( QVariant oName, QVariant oValue )
*/
void CATypesetCtl::exportDocument( CADocument *poDoc )
{
	// @todo: Add export options to the document directly ?
	if( _poExport )
	{
		QTemporaryFile::createLocalFile( _oOutputFile ); 
		_poExport->setStreamToFile( _oOutputFile.fileName() );
		_poExport->exportDocument( poDoc );
	}
	else
	  qCritical("TypesetCtl: No export was done - no exporter defined");
}

/*!

*/
void CATypesetCtl::runTypesetter()
{
	if( _poTypesetter->execProgram() )
	{
		if( _bPDFConversion )
		{
			if( !createPDF() )
		  	qCritical("TypesetCtl: Creating pdf file failed!");
		}
	}
	else
	  qCritical("TypesetCtl: Running typesetter failed!");
}

/*!
	Runs the conversion from postscript to pdf in the background

	Currently neither progress nor output messages are supported
	when doing this conversion step. Only startup failures are handled.
*/
bool CATypesetCtl::createPDF()
{
	return _poConvPS2PDF->execProgram();
}

void CATypesetCtl::rcvTypesetterOutput()
{
}

void CATypesetCtl::typsetterFinished( int iExitCode )
{
	if( iExitCode != 0 )
	  qCritical("TypesetCtl: Typesetter finished with code %d",iExitCode);
}

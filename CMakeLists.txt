# --------------------------------------------------
# Project name
SET( ProjectName Diverse )
PROJECT( ${ProjectName} )

# --------------------------------------------------
# Dependencies

cmake_minimum_required( VERSION 2.4.0 )

IF( COMMAND cmake_policy )
	cmake_policy( SET CMP0003 NEW )
ENDIF( COMMAND cmake_policy )

# We need NQVTK
FIND_PACKAGE( NQVTK REQUIRED )
INCLUDE( ${NQVTK_USE_FILE} )

# We need NQVTKGui
FIND_PACKAGE( NQVTKGui REQUIRED )
INCLUDE( ${NQVTKGui_USE_FILE} )

# We're using GLEW, which is provided by NQVTK. However, we still need 
# to set this as we're using some GLEW functions directly.
ADD_DEFINITIONS( -DGLEW_STATIC )

# We need VTK for its readers
FIND_PACKAGE( VTK REQUIRED )
INCLUDE( ${VTK_USE_FILE} )
SET( VTK_lib
	vtkIO
)

# We need Qt
FIND_PACKAGE( Qt4 REQUIRED )
SET( QT_USE_QTMAIN TRUE )
SET( QT_USE_QTOPENGL TRUE )
INCLUDE( ${QT_USE_FILE} )
# Required Qt modules
SET( QT_lib
	${QT_LIBRARIES}
	${QT_QTMAIN_LIBRARIES}
)

# The complete set of libraries required by the project
SET( ${ProjectName}_Libraries
	${NQVTK_LIBRARIES}
	${NQVTKGui_LIBRARIES}
	${VTK_lib}
	${QT_lib}
)

# --------------------------------------------------
# Modules

SET( ${ProjectName}_SourceBaseDir
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

SET( ${ProjectName}_Modules
	Gui
	Data
	Rendering
)

# Friendly names for the modules
SET( ${ProjectName}_ModuleNames
	"GUI"
	"Data"
	"Rendering"
)

# --------------------------------------------------
# Classes for each of the modules

SET( ${ProjectName}_Gui_Classes
	MainWindow
	MainViewer
)

SET( ${ProjectName}_Data_Classes
	ShapeModel
)

SET( ${ProjectName}_Rendering_Classes
	ContoursRenderer
)

# --------------------------------------------------
# Extra source files (e.g. program main)
SET( ${ProjectName}_ExtraSource
	main.cpp
)

# Application icon on Windows builds
IF( WIN32 )
	SET( ${ProjectName}_ExtraSource
		${${ProjectName}_ExtraSource}
	)
ENDIF( WIN32 )

# ==================================================
# YOU DO NOT NEED TO MODIFY ANYTHING BELOW THIS LINE
# ==================================================

# --------------------------------------------------
# Macros

MACRO( collect_class_source_files module class headers sources )
	# Find header file
	SET( headerfile "${${ProjectName}_SourceBaseDir}/${module}/${class}.h" )
	IF( EXISTS ${headerfile} )
		SET( ${headers} ${${headers}} ${headerfile} )
	ELSE( EXISTS ${headerfile} )
		SET( headerfile "${${ProjectName}_SourceBaseDir}/${class}.h" )
		IF( EXISTS ${headerfile} )
			SET( ${headers} ${${headers}} ${headerfile} )
		ELSE( EXISTS ${headerfile} )
			MESSAGE( SEND_ERROR "Class header not found for '${class}'" )
		ENDIF( EXISTS ${headerfile} )
	ENDIF( EXISTS ${headerfile} )

	# Find implementation file
	SET( classfile "${${ProjectName}_SourceBaseDir}/${module}/${class}.cpp" )
	IF( EXISTS ${classfile} )
		SET( ${sources} ${${sources}} "${classfile}" )
	ELSE( EXISTS ${classfile} )
		SET( classfile "${${ProjectName}_SourceBaseDir}/${module}/${class}.cxx" )
		IF( EXISTS ${classfile} )
			SET( ${sources} ${${sources}} "${classfile}" )
		ELSE( EXISTS ${classfile} )
			SET( classfile "${${ProjectName}_SourceBaseDir}/${class}.cpp" )
			IF( EXISTS ${classfile} )
				SET( ${sources} ${${sources}} "${classfile}" )
			ELSE( EXISTS ${classfile} )
				SET( classfile "${${ProjectName}_SourceBaseDir}/${class}.cxx" )
				IF( EXISTS ${classfile} )
					SET( ${sources} ${${sources}} "${classfile}" )
				ELSE( EXISTS ${classfile} )
					# Allow header-only implementations
					# MESSAGE( SEND_ERROR "Class implementation not found for '${class}'" )
				ENDIF( EXISTS ${classfile} )
			ENDIF( EXISTS ${classfile} )
		ENDIF( EXISTS ${classfile} )
	ENDIF( EXISTS ${classfile} )

	# Find ui file
	SET( uiheader )
	SET( uifile "${${ProjectName}_SourceBaseDir}/${module}/${class}.ui" )
	IF( EXISTS ${uifile} )
		QT4_WRAP_UI( uiheader ${uifile} )
		SET( ${headers} ${${headers}} ${uiheader} )
	ELSE( EXISTS ${uifile} )
		SET( classfile "${${ProjectName}_SourceBaseDir}/${class}.ui" )
		IF( EXISTS ${uifile} )
			QT4_WRAP_UI( uiheader ${uifile} )
			SET( ${headers} ${${headers}} ${uiheader} )
		ENDIF( EXISTS ${uifile} )
	ENDIF( EXISTS ${uifile} )
ENDMACRO( collect_class_source_files )

# --------------------------------------------------
# Prepare sets

SET( ${ProjectName}_Headers )
SET( ${ProjectName}_Source ${${ProjectName}_ExtraSource} )
SET( ${ProjectName}_GeneratedSource )

# --------------------------------------------------
# Collect source files and create source groups

# Iterate over the modules
LIST( LENGTH ${ProjectName}_Modules nmodules )
MATH( EXPR nmodules "${nmodules} - 1" )
FOREACH( i RANGE ${nmodules} )

	# Fetch name and label of the module
	LIST( GET ${ProjectName}_Modules ${i} module )
	LIST( GET ${ProjectName}_ModuleNames ${i} modulename )

	# See if there is a separate CMake include for this module
	SET( cmakeincludefile "${${ProjectName}_SourceBaseDir}/${module}/sources.cmake" )
	IF( EXISTS ${cmakeincludefile} )
		# Include the file
		INCLUDE( ${cmakeincludefile} )
	ELSE( EXISTS ${cmakeincludefile} )
		# Iterate over classes
		SET( ${ProjectName}_${module}_Headers )
		SET( ${ProjectName}_${module}_Source )
		SET( ${ProjectName}_${module}_GeneratedSource )

		FOREACH( class ${${ProjectName}_${module}_Classes} )
			collect_class_source_files( 
				${module} 
				${class} 
				${ProjectName}_${module}_Headers 
				${ProjectName}_${module}_Source 
			)
		ENDFOREACH( class )
		
		# Qt MOC
		QT4_AUTOMOC( ${${ProjectName}_${module}_Source} )
	ENDIF( EXISTS ${cmakeincludefile} )

	# Create source group for this module
	# Generated sources don't need to show up in this group, headers do
	SOURCE_GROUP( ${modulename} 
		FILES ${${ProjectName}_${module}_Source} ${${ProjectName}_${module}_Headers} 
	)
	# Add a source group for the .ui files
	SOURCE_GROUP( "${modulename}\\Qt UI Files" 
		REGULAR_EXPRESSION "/${module}/.*\\.ui\$" 
	)
	# Append files to complete sets for the project
	SET( ${ProjectName}_Headers 
		${${ProjectName}_Headers} 
		${${ProjectName}_${module}_Headers} 
	)
	SET( ${ProjectName}_Source
		${${ProjectName}_Source} 
		${${ProjectName}_${module}_Source} 
		${${ProjectName}_${module}_Headers} # Add headers as well
	)
	SET( ${ProjectName}_GeneratedSource
		${${ProjectName}_GeneratedSource}
		${${ProjectName}_${module}_GeneratedSource}
	)
ENDFOREACH( i )

# Add generated sources and give them a source group
SET( ${ProjectName}_Source
	${${ProjectName}_Source}
	${${ProjectName}_GeneratedSource}
)
SOURCE_GROUP( "Generated files"
	FILES ${${ProjectName}_GeneratedSource}
)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${${ProjectName}_SourceBaseDir} )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

# --------------------------------------------------
# Executable target

ADD_EXECUTABLE( 
	${ProjectName}
	${${ProjectName}_Source}
)

TARGET_LINK_LIBRARIES(
	${ProjectName} 
	${${ProjectName}_Libraries}
)

#pragma once

#include <NQVTKGui/Viewports/NQVTKWidget.h>

#include <itpp/itbase.h>

namespace NQVTK { class Scene; }

namespace Diverse
{
	class ShapeModel;
	class LinearPopulationTrajectory;
	class LinearMorphTrajectory;
	class EvolutionConfiguration;

	class ShapeEvolutionViewer : public NQVTKWidget
	{
		Q_OBJECT;

	public:
		ShapeEvolutionViewer(QWidget *parent = 0);
		~ShapeEvolutionViewer();

		void SetShapeModel(ShapeModel *model);

		void SetConfiguration(EvolutionConfiguration *config);

		enum ColorMode
		{
			ColorByDeformation,
			ColorByReconstructionError
		};

		void SetColorMode(ColorMode mode);

	public slots:
		void SetVector(itpp::vec vector);
		void SyncMeshCamera(NQVTK::Camera *cam);
		void SetReconstructionDimension(int dims);

		void MorphShape(const itpp::vec &shape);
		void MorphEnd();

	protected:
		ShapeModel *model;

		ColorMode colorMode;

		LinearPopulationTrajectory *trajectory;
		LinearMorphTrajectory *morph;
		EvolutionConfiguration *configuration;

		int reconstructionDims;

		void UpdateReference();
	};
}

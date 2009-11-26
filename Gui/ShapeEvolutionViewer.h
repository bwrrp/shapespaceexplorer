#pragma once

#include <NQVTKGui/Viewports/NQVTKWidget.h>

#include <itpp/itbase.h>

namespace NQVTK { class Scene; }

namespace Diverse
{
	class ShapeModel;
	class LinearPopulationTrajectory;
	class EvolutionConfiguration;

	class ShapeEvolutionViewer : public NQVTKWidget
	{
		Q_OBJECT;

	public:
		ShapeEvolutionViewer(QWidget *parent = 0);
		~ShapeEvolutionViewer();

		void SetShapeModel(ShapeModel *model);

	public slots:
		void SetVector(itpp::vec vector);
		void SyncMeshCamera(NQVTK::Camera *cam);

	protected:
		ShapeModel *model;

		// TODO: make the trajectory type configurable
		LinearPopulationTrajectory *trajectory;
		EvolutionConfiguration *configuration;
	};
}

#pragma once

#include <NQVTKGui/Viewports/NQVTKWidget.h>

#include <itpp/itbase.h>

namespace NQVTK { class Scene; }

namespace Diverse
{
	class ShapeModel;

	class MeshViewer : public NQVTKWidget
	{
		Q_OBJECT;

	public:
		MeshViewer(QWidget *parent = 0);
		virtual ~MeshViewer();

		void SetShapeModel(ShapeModel *model);

		enum ColorMode
		{
			ColorByDeformation,
			ColorByReconstructionError
		};

		void SetColorMode(ColorMode mode);

		const itpp::vec GetShape();

	public slots:
		void SetShape(itpp::vec shape);
		void SetReconstructionDimension(int dims);

	protected:
		ShapeModel *model;
		NQVTK::Scene *scene;

		ColorMode colorMode;
		int reconstructionDims;

		void UpdateReference();
	};
}

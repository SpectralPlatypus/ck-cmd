#pragma once

#include <src/hkx/ResourceManager.h>
#include <src/models/ProjectModel.h>

#include <QObject>
#include <QMenu>
#include <QTreeView>

class FBXImport;

namespace ckcmd {
	namespace HKX {

		class ActionHandler : public QObject
		{
			Q_OBJECT

			ProjectModel& _model;

			QAction* _save;

			QAction* _createProject;

			QAction* _importFBX;
			QAction* _removeAnimation;

			QAction* _copy;

			void buildSaveAction();
			void buildImportFBXAction();
			void buildRemoveAnimation();
			void buildCreateProjectAction();
			void buildCopyAction();

		public slots:
			void save();

			void createProject();
			void importFBX();
			void removeAnimation();
			void copy();

		public:
			ActionHandler(ProjectModel& model, QObject* parent = nullptr) :
				QObject(parent),
				_save(nullptr),
				_model(model)
			{
				buildSaveAction();
				buildImportFBXAction();
				buildRemoveAnimation();
				buildCopyAction();
			}



			QAction* saveAction(const QVariant& action_data);
			QAction* exportAction();
			QAction* importFBXAction() { return _importFBX; };
			QAction* removeAnimationAction() { return _removeAnimation; }
			QAction* createProjectAction() { return _createProject; }
			QAction* copyAction() { return _copy; }

		};
	}
}
#pragma once

#include <src/hkx/ResourceManager.h>
#include <src/models/ModelEdge.h>

#include <QVariant>

struct hkVariant;

namespace ckcmd {
	namespace HKX {

		class ProjectTreeHkHandler
		{

		public:

			static int childRows(int row, int column, const ModelEdge& edge, ResourceManager& manager);
			static int childColumns(int row, int column, const ModelEdge& edge, ResourceManager& manager);
			static int childCount(const ModelEdge& edge, ResourceManager& manager);
			static bool hasChild(int row, int column, const ModelEdge& edge, ResourceManager& manager);
			static ModelEdge child(int row, int column, const ModelEdge& edge, ResourceManager& manager);

			static QVariant data(int row, int column, const ModelEdge& edge, ResourceManager& manager);
			static bool setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager);
			static bool addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
			static bool removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
			static bool changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager);
		};
	}
}
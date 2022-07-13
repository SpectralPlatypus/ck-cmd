#pragma once

#include <src/models/ModelEdge.h>
#include <src/edges/MultipleVariantsEdge.h>

#include <hkbClipGenerator_2.h>

namespace ckcmd {
	namespace HKX {

		class ClipModel : public MultipleVariantsEdge {

			hkbClipGenerator* variant(const ModelEdge& edge) const;
			virtual std::vector<std::function<hkVariant* (const ModelEdge&, ResourceManager& manager, hkVariant*)>> additional_variants() const override;
		
		public:
			virtual std::vector<NodeType> handled_types() const override { return {}; }
			virtual std::vector<const hkClass*> handled_hkclasses() const override;
		};
	}
}
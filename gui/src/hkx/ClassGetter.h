#include "HkxItemVisitor.h"
#include <QString>

namespace ckcmd {
	namespace HKX {

		class ClassGetter : public HkxConcreteVisitor<ClassGetter> {
			
			const hkClass* _class;
			int _rows = 0;
			int _target_row;

			const hkClassMember& getSerializedMember(size_t row);

		public:

			ClassGetter(const int row) : 
				HkxConcreteVisitor(*this), 
				_class(nullptr),
				_target_row(row)
			{}

			const hkClass* hkclass() { return _class; }

			template<typename T> void visit(T& value);

			virtual void visit(char* value) override;
			virtual void visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags) override;
			virtual void visit(void* object, const hkClassMember& definition) override;
			virtual void visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) override;
			virtual void visit(void* object, const hkClass& object_type, const char* member_name) override;
			virtual void visit(void* v, const hkClassEnum& enum_type, size_t storage) override;

		};
	}
}
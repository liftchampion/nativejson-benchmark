// product1.cpp

#include "product.h"

class U_EXPORT Product1 : public Product {
public:
            Product1() {}
   virtual ~Product1() { (void) write(1, U_CONSTANT_TO_PARAM("distruttore Product1\n")); }

   virtual void print(ostream& os) const { os << "I am Product1" << endl; }
};

U_CREAT_FUNC(product1, Product1)

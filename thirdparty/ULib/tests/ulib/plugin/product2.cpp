// product2.cpp

#include "product.h"

class U_EXPORT Product2 : public Product {
public:
            Product2() {}
   virtual ~Product2() { (void) write(1, U_CONSTANT_TO_PARAM("distruttore Product2\n")); }

   virtual void print(ostream& os) const { os << "I am Product2" << endl; }
};

U_CREAT_FUNC(product2, Product2)

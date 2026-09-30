#ifndef MOVIE_H
#define MOVIE_H

#include "product.h"
#include <string>
#include <set>

using namespace std;

class Movie : public Product {
  public:
    Movie(const string category, const string name, double price, int qty, const string genre, const string rating);

    virtual ~Movie();

    virtual std::set<std::string> keywords() const;

    virtual std::string displayString() const;

    virtual void dump(std::ostream& os) const;
  
  private:
    std::string genre_;
    std::string rating_;

};

#endif
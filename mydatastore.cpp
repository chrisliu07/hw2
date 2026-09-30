#include "mydatastore.h"
#include "util.h"

#include <iostream>

using namespace std;

MyDataStore::MyDataStore() {}

MyDataStore::~MyDataStore() {
  for(vector<Product*>::iterator it = products_.begin(); it != products_.end(); ++it) {
    delete *it;
  }

  for (map<string, User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
    delete it->second;
  }
}


void MyDataStore::addProduct(Product* p) {
  products_.push_back(p);
  set<string> keys = p->keywords();

  for (set<string>::iterator it = keys.begin(); it != keys.end(); ++it) {
    string key = convToLower(*it);
    keywordMap_[key].insert(p);
  }
}

void MyDataStore::addUser(User* u) {
  string username = convToLower(u->getName());

  users_[username] = u;
  carts_[username] = vector<Product*>();
}

vector<Product*> MyDataStore::search(vector<string>& terms, int type) {
  vector<Product*> resultVector;
  set<Product*> resultSet;

  if (terms.size() == 0) {
    return resultVector;
  }

  if (type == 0) {
    string firstTerm = convToLower(terms[0]);

    map<string, set<Product*>>::iterator first = keywordMap_.find(firstTerm);

    if (first == keywordMap_.end()) { return resultVector; }

    resultSet = first->second;

    for (unsigned int i = 1; i < terms.size(); i++) {
      string term = convToLower(terms[i]);

      map<string, set<Product*>>::iterator it = keywordMap_.find(term);

      if(it == keywordMap_.end()) {
        resultSet.clear();
        break;
      }

      set<Product*> temp = it->second;

      resultSet = setIntersection(resultSet, temp);
    }
  }

  else if (type == 1) {
    for (unsigned int i = 0; i < terms.size(); i++) {
      string term = convToLower(terms[i]);

      map<string, set<Product*>>::iterator it = keywordMap_.find(term);

      if(it != keywordMap_.end()) {
        set<Product*> temp = it->second;

        resultSet = setUnion(resultSet, temp);
      }
    }
  } 

  for (set<Product*>::iterator it = resultSet.begin(); it!= resultSet.end(); ++it) {
    resultVector.push_back(*it);
  } 

  return resultVector;
}

void MyDataStore::dump(ostream& ofile) {
  ofile << "<products>" << endl;

  for (vector<Product*>::iterator it = products_.begin(); it != products_.end(); ++it) {
    (*it)->dump(ofile);
  }

  ofile << "</products>" << endl;

  ofile << "<users>" << endl;
  
  for (map<string, User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
    it->second->dump(ofile);
  }

  ofile << "</users>" << endl;
}

bool MyDataStore::addToCart(const string& username, Product* product) {
  string lowerUsername = convToLower(username);

  map<string, User*>::iterator userIt = users_.find(lowerUsername);

  if(userIt == users_.end()) {
    return false;
  }

  carts_[lowerUsername].push_back(product);

  return true;
}

bool MyDataStore::viewCart(const string& username) {
  string lowerUsername = convToLower(username);

  map<string, User*>::iterator userIt = users_.find(lowerUsername);

  if (userIt == users_.end()) {
    return false;
  }

  vector<Product*>& cart = carts_[lowerUsername];

  for (unsigned int i = 0; i < cart.size(); i++) {
    cout << "Item " << i + 1 << endl;
    cout << cart[i]->displayString() << endl; 
  }

  return true;
} 

bool MyDataStore::buyCart(const string& username) {
  string lowerUsername = convToLower(username);

  map<string, User*>::iterator userIt = users_.find(lowerUsername);

  if (userIt == users_.end()) {
    return false;
  }

  User* user = userIt->second;

  vector<Product*>& cart = carts_[lowerUsername];

  vector<Product*> remainingCart;

  for (unsigned int i = 0; i < cart.size(); i++) {
    Product* product = cart[i];

    if (product->getQty() > 0 && user->getBalance() >= product->getPrice()) {
      product->subtractQty(1);
      user->deductAmount(product->getPrice());
    } else {
      remainingCart.push_back(product);
    }
  }

  cart = remainingCart;
  return true;
}
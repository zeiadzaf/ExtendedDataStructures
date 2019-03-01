#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <initializer_list>
#include <vector>

class User;
class SocialNetwork;

class UserTypes {
public:
    typedef unsigned long long id_type;
    typedef unsigned int       height_type;
    typedef unsigned char      age_type;
    
    enum Sex { MALE, FEMALE, UNSPECIFIED};
};

class UserChangesListener {
public:
    void init(User* u)  {
        user = u;
    }

    User* getUser() {
        return user;
    }
    
    void notify_setAge(UserTypes::age_type old, UserTypes::age_type age);
    void notify_removeHobby(std::string h);
    void notify_addHobby();
    
private:
    User* user;
};


class UserViewIF {
public:
    virtual UserTypes::id_type getID() = 0;
    virtual std::string getName()  = 0;
    virtual UserTypes::height_type getHeight() = 0;
    virtual std::string getSex() = 0;
    virtual UserTypes::age_type getAge() = 0;
    virtual std::unordered_set<UserTypes::id_type> getFriends() = 0;
    virtual std::unordered_set<std::string> getHobbies() = 0;
    virtual void dump() = 0;
};


class UserModifyIF {
public:
    virtual void setAge(UserTypes::age_type a) = 0;
    virtual void setHeight(UserTypes::height_type h) = 0;
    virtual void setSex(std::string s) = 0;
    virtual void setSex(UserTypes::Sex s) = 0;
    virtual void removeHobby(std::string h) = 0;
    virtual void addHobbies(std::initializer_list<std::string> h) = 0;
	virtual void addHobby(std::string h) = 0;
    virtual void removeFriend(UserTypes::id_type f) = 0;
    virtual void addFriend(UserTypes::id_type f) = 0;
};

class User : public UserViewIF, public UserModifyIF {
private:
    User(std::string name) : 
      id(gid++),
      name(name),
      age(0),
      height(0),
      type(UserTypes::Sex::UNSPECIFIED) {
          listener.init(this);
    }
    
    User(std::string name, std::initializer_list<std::string> info) :
      id(gid++),
      name(name) {
        listener.init(this);
        auto it = info.begin();
        if(it==info.end()) return;
        age = stoi(*it); ++it;
        if(it==info.end()) return;
        height = stoi(*it); ++it;
        if(it==info.end()) return;
        type = it->compare("MALE") == 0 ? UserTypes::Sex::MALE : (it->compare("FEMALE") == 0 ? UserTypes::Sex::FEMALE : UserTypes::Sex::UNSPECIFIED); 
        ++it;
        hobbies.insert(it, info.end());
    }
    
public:
    UserTypes::id_type getID() {
        return id;
    }
    
    std::string getName() {
        return name;
    }
    
    void setAge(UserTypes::age_type a) {
        listener.notify_setAge(age, a);
        age = a;
    }
    
    UserTypes::age_type getAge() {
        return age;
    }
    
    void setHeight(UserTypes::height_type h) {
        height = h;
    }

    UserTypes::height_type getHeight() {
        return height;
    }
    
    void setSex(std::string s) {
        type = s.compare("MALE") == 0 ? UserTypes::Sex::MALE : UserTypes::Sex::FEMALE;
    }
    
    void setSex(UserTypes::Sex s) {
        type = s;
    }
    
    std::string getSex() {
        return type == UserTypes::Sex::UNSPECIFIED ? "UNSPECIFIED" : (type == UserTypes::Sex::MALE ? "MALE" : "FEMALE");
    }
    
    void removeHobby(std::string h) {
        auto it = hobbies.find(h);
        if(it != hobbies.end()) {
            listener.notify_removeHobby(h);
            hobbies.erase(it);
        }
    }
    
    void addHobbies(std::initializer_list<std::string> h) {
        listener.notify_addHobby();
        hobbies.insert(h.begin(), h.end());
    }
    
    void addHobby(std::string h) {
        listener.notify_addHobby();
        hobbies.insert(h);
    }
    
    std::unordered_set<std::string> getHobbies() {
        return hobbies;
    }
    
    void removeFriend(UserTypes::id_type f) {
        auto it = friends.find(f);
        if(it != friends.end())
            friends.erase(it);
    }
    
    void addFriend(UserTypes::id_type f) {
        friends.insert(f);
    }
    
    std::unordered_set<UserTypes::id_type> getFriends() {
        return friends;
    }
    
    void dump() {
        std::cout << "User " << id << " info:\n";
        std::cout << "name= " << name << std::endl;
        std::cout << "age = " << (unsigned int)age << std::endl;
        std::cout << "height = " << height << std::endl;
        std::cout << "sex = " << getSex() << std::endl;
        std::cout << "hobbies = {";
        for(auto& h : hobbies)
            std::cout << "{" << h << "} ";
        std::cout << "}\n";

        std::cout << "friends = {";
        for(auto& f : friends)
            std::cout << "{" << f << "} ";
        std::cout << "}\n";        
    }
    
private:
    std::unordered_set<std::string>& getHobbiesRef() {
        return hobbies;
    }
    
private:
    const UserTypes::id_type id;
    std::string name;
    UserTypes::age_type age;
    UserTypes::height_type height;
    UserTypes::Sex type;
    std::unordered_set<std::string> hobbies;
    std::unordered_set<UserTypes::id_type> friends;
private:
    static UserTypes::id_type gid;
    
private:
    UserChangesListener    listener;
    
    friend class SocialNetwork;
};


class SocialNetwork {
public:
    static SocialNetwork* getHandle() {
        if(singleton == nullptr)
            singleton = new SocialNetwork;
        return singleton;
    }
    
    ~SocialNetwork() {
        for(auto& p : users)
            delete p.second;
    }
    
private:
    static SocialNetwork* singleton;    
    SocialNetwork() {}
    
public:
    UserModifyIF* addUser(std::string name) {
        User* u = new User(name);
        addUser(u);
        return u;
    }
    
    UserModifyIF* addUser(std::string name, std::initializer_list<std::string> info) {
        User* u = new User(name, info);
        addUser(u);
        return u;
    }
    
    void deleteUser(UserTypes::id_type id) {
        auto it = users.find(id);
        if(it == users.end())
            return;
        deletedUsers.insert(id);
        
        auto u = it->second;
        
        auto it2 = usersByName.find(u->getName());
        it2->second.erase(it2->second.find(u));
        
        auto it3 = usersByAge.find(u->getAge());
        it3->second.erase(it3->second.find(u));
        
        auto hobbies = u->getHobbiesRef();
        for(auto& h : hobbies) {
            auto it4 = usersByHobby.find(h);
            it4->second.erase(it4->second.find(u));
        }
        
        users.erase(it);
        delete u;
    }
    
    std::vector<UserViewIF*> searchUserByName(std::string name) {
        std::vector<UserViewIF*> result;
        auto it = usersByName.find(name);
        if(it != usersByName.end())
            result.insert(result.begin(), it->second.begin(), it->second.end());
        return result;
    }
    
    std::vector<UserViewIF*> searchUserByAge(UserTypes::age_type age) {
        std::vector<UserViewIF*> result;
        auto it = usersByAge.find(age);
        if(it != usersByAge.end())
            result.insert(result.begin(), it->second.begin(), it->second.end());        
        return result;
    }
    
    std::vector<UserViewIF*> searchUserByHobbies(std::initializer_list<std::string> hobbies) {
        std::vector<UserViewIF*> result;
        auto hobbiesIt = hobbies.begin();
        auto it = usersByHobby.find(*hobbiesIt);
        if(it == usersByHobby.end())
            return result;
        auto ret = it->second;
        ++hobbiesIt;
        while(hobbiesIt != hobbies.end()) {
            it = usersByHobby.find(*hobbiesIt);
            if(it == usersByHobby.end()) return result;
            decltype(ret) tmp;
            for(auto& v : ret) {
                if(it->second.find(v) != it->second.end())
                    tmp.insert(v);
            }
            ++hobbiesIt;
            ret = tmp;
        }

        result.insert(result.begin(), ret.begin(), ret.end());
        
        return result;
    }
    
    std::vector<UserTypes::id_type> getFriendsOfUser(UserTypes::id_type id) {
        std::vector<UserTypes::id_type> result;
        auto it = users.find(id);
        if(it != users.end()) { //check if user exist
            auto friends = it->second->getFriends();
            result.reserve(friends.size());
            for(auto& f : friends) {
                if(deletedUsers.find(f) == deletedUsers.end()) //check if that friend already deleted his account
                    result.push_back(f);
                else
                    it->second->removeFriend(f);
            }
        }
        return result;
    }
    
private:
    void addUser(User* u) {
        users.insert({u->getID(), u});
        usersByName[u->getName()].insert(u);
        usersByAge[u->getAge()].insert(u);
        
        auto hobbies = u->getHobbiesRef();
        for(auto& h : hobbies)
            usersByHobby[h].insert(u);
    }
    
    void setAge(User* u, UserTypes::age_type old, UserTypes::age_type age) {
        auto it = usersByAge.find(old);
        it->second.erase(it->second.find(u));
        usersByAge[age].insert(u);
    }
    
    void removeHobby(User* u, std::string h) {
        auto it = usersByHobby.find(h);
        it->second.erase(it->second.find(u));
    }
    
    void addHobby(User* u) {
        auto hobbies = u->getHobbiesRef();
        for(auto& h : hobbies)
            usersByHobby[h].insert(u);        
    }
    
private:
    typedef std::unordered_set<User*> userSetType;
    
    std::unordered_set<UserTypes::id_type> deletedUsers;
    std::unordered_map<UserTypes::id_type, User*> users;
    std::unordered_map<std::string, userSetType> usersByName;
    std::unordered_map<UserTypes::age_type, userSetType> usersByAge;
    std::unordered_map<std::string, userSetType> usersByHobby;
    
    friend class UserChangesListener;
};

SocialNetwork* SocialNetwork::singleton = nullptr;
UserTypes::id_type User::gid = 0;


void UserChangesListener::notify_setAge(UserTypes::age_type old, UserTypes::age_type age) {
    SocialNetwork::getHandle()->setAge(getUser(), old, age);
}

void UserChangesListener::notify_removeHobby(std::string h) {
    SocialNetwork::getHandle()->removeHobby(getUser(), h);    
}

void UserChangesListener::notify_addHobby() {
    SocialNetwork::getHandle()->addHobby(getUser());
}



int main() {
    SocialNetwork* handle= SocialNetwork::getHandle();
    auto usr1 = handle->addUser("Zeiad", {"26", "200", "MALE", "running", "squash", "fishing"});
    auto usr2 = handle->addUser("Zeiad", {"20", "7"});
    auto usr3 = handle->addUser("Another");
    
    std::cout << "Get by Name: Zeiad\n";
    auto result = handle->searchUserByName("Zeiad");
    for(auto r : result) {
        r->dump();
        std::cout << std::endl;
    }

    
    std::cout << "Get by Age = 26\n";
    result = handle->searchUserByAge(26);
    for(auto r : result) {
        r->dump();
        std::cout << std::endl;
    }    
    
    std::cout << "Modify user with id =1 age to 26\n";    
    usr2->setAge(26);
    
    std::cout << "Get by Age = 26\n";
    result = handle->searchUserByAge(26);
    for(auto r : result) {
        r->dump();
        std::cout << std::endl;
    }    
    
    std::cout << "Get by Age = 20\n";
    result = handle->searchUserByAge(20);
    for(auto r : result) {
        r->dump();
        std::cout << std::endl;
    }
    
    std::cout << "Add ids 0,1 as friend for user with id 2\n";
    usr3->addFriend(0);
    usr3->addFriend(1);
    
    std::cout << "Get by Name: Another\n";    
    result = handle->searchUserByName("Another");
    for(auto r : result) {
        r->dump();
        std::cout << std::endl;
    }
    
    std::cout << "delete user with id=0 \n\n";
    handle->deleteUser(0);

    std::cout << "Get by Name: Zeiad\n";
    result = handle->searchUserByName("Zeiad");
    for(auto r : result) {
        r->dump();
        std::cout << std::endl;
    }

    std::cout << "Get friends of user: Another\n";    
    auto res = handle->getFriendsOfUser(2);
    for(auto r : res) {
        std::cout << r << std::endl;
    }
    
    delete handle;
}
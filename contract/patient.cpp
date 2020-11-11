#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

class [[eosio::contract("patient")]] parkingspot : public eosio::contract {
private:
  uint64_t now() {
    return current_time_point().sec_since_epoch();
  }

  //Time
  uint64_t start = now();

  //Patient information
  struct [[eosio::table]] patient {
    uint64_t Patient_ID;
    name Name;
    string Gender;
    uint64_t DOB; //TODO Change to time_point
    uint16_t Contact_ID;
    uint16_t Emergency_ID;
    uint64_t Primary_ID; 
    uint64_t Vital_ID;

    //Sets primary key to be the Patient_ID
    uint64_t  primary_key() const {
      return Patient_ID;
    }

    //TODO Make Name secondary index
    
  };

  //Contact Information
  struct [[eosio::table]] contact {
    uint64_t Contact_ID;
    string Address;
    string Phone;
    string Email;

    //Sets primary key to be the Contact_ID
    uint64_t  primary_key() const {
      return Contact_ID;
    }
    
  };

  //Emergency Information
  struct [[eosio::table]] emergency {
    uint64_t Emergency_ID;
    string Name;
    string Relationship;
    uint64_t Contact_ID;

    //Sets primary key to be the Emergency_ID
    uint64_t  primary_key() const {
      return Emergency_ID;
    }
    
  };

  //Medical Professional Information
  struct [[eosio::table]] medical {
    uint64_t Admin_ID;
    name Name;
    string Hours;
    uint64_t Contact_ID;

    //Sets primary key to be the Admin_ID
    uint64_t  primary_key() const {
      return Admin_ID;
    }
    
  };

  //Patient Vital Information
  struct [[eosio::table]] vital {
    uint64_t Vital_ID;
    float64 BodyTemp;
    float64 PulseRate;
    float64 RespirationRate;
    float64 BloodPressure;
    uint64_t LastModified;

    //Sets primary key to be the Vital_ID
    uint64_t  primary_key() const {
      return Vital_ID;
    }
    
  };
  void send_summary(name user, std::string message) {
    /*
    * Permision level = authorized is the active authority of the contract
    * Code = account where contract is deployed
    * Action we want to perform
    * Data to pass to the action
    */
    action (
      permission_level{get_self(),"active"_n},
      get_self(),
      "notify"_n,
      std::make_tuple(user, name{user}.to_string() + message)
    ).send();
  };


public:
  using contract::contract; //?

  [[eosio::action]]
  void notify(name user, std::string msg) {
    //Only authorization is the contract itself
    require_auth(get_self());
    //Ensures the accounts receive the notificaiton of the action being executed (carbon copy)
    require_recipient(user);
  }

  //Inserts new patient
  //Assumes new contact, emergency, and vital
  //User = Medical pro (Admin)
  [[eosio::action]] //Needed for ABI generation
  void insert(name user, 
                  name patient,
                  string gender, 
                  string dob, 
                  string address, 
                  string phone,
                  string email
                  string emergency,
                  string relationship) {
      //Ensures the account executing transaction has proper permissions
      require_auth(user);
      
      //TODO Insert Contact
      //TODO Insert Emergency
      //TODO Check for doctor ID
      //TODO Insert to Vitals (initially 0) 
      /* Creates Table to index from
      *  first param specifies the owner of this table
      *     in this case the name of the contract
      *  second param is the scope and ensures uniqueness of the table within the contract
      *     in this case the account name this contract is deployed to
      */
      patient_index patient(get_self(), get_first_receiver().value);

      //Iterator for patient using patient_id as key
      uint128_t  key_int = 0//(spot_id*1000000000)+(time_code*1000)+zone_id; 
      
      //Iterate with secondary key
      //auto secparkdeck = parkdeck.get_index<name("seckey")>();
      
      auto iterator = secpatient.find(key_int);
      if( iterator == secparkdeck.end()) {
            //The parking spot isn't in the table
            patient.emplace(user, [&](auto& row) {
                row.Patient_ID = patient.available_primary_key();
                row.Name = patient;
                row.Gender = gender;
                row.DOB = dob;
            });
            print("Patient: ",patient" is created on: ", now());
            send_summary(user, " successfully inserted patient");
        }
        else {
              //The patient is in the table
              print("ALREADY EXISTS! Patient: ", patient);
        }
  }

  //Erases parking spot 
  [[eosio::action]]
  void erase(name user, uint16_t spot_id, uint16_t zone_id, uint64_t time_code) {
    require_auth(user);

    park_index parkdeck(get_self(), get_first_receiver().value);

    //Iterator for parking spots using spot_id as key
    uint128_t  key_int = (spot_id*10000000000000)+(time_code*1000)+zone_id; 


    //Iterate with secondary key
    auto secparkdeck = parkdeck.get_index<name("seckey")>();
    auto iterator = secparkdeck.find(key_int);

    if( iterator == secparkdeck.end() ) {
          //The parking spot isn't in the table
          //    check(iterator != addresses.end(), "Record does not exist");
          print("DOES NOT EXIST! Parking Spot: ", spot_id, " in Zone: ", zone_id);
      }
      else {
          secparkdeck.erase(iterator);
          print("REMOVED Parking Spot: ", spot_id, "  on: ", now());
          send_summary(user, " successfully removed parking spot");

      }
    
  }

    //Erases parking spot 
  [[eosio::action]]
  void clear(name user) {
    require_auth(user);

    park_index patient(get_self(), get_first_receiver().value);
    auto it = patient.begin();
    while (it != patient.end()) {
        it = patient.erase(it);
    }
    //TODO Clear Contacts
    //TODO Clear Emergency
    //TODO Clear Medical Professional
    //TODO Clear Vitals
    
  }

  //TODO Corresponding Updates

};

#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

class [[eosio::contract("patient")]] patient : public eosio::contract {
private:
  uint64_t now() {
    return current_time_point().sec_since_epoch();
  }

  //Time
  uint64_t start = now();

  //Patient information
  struct [[eosio::table]] patients {
    uint64_t Patient_ID;
    name Name;
    std::string Gender;
    std::string DOB; //TODO Change to time_point
    uint16_t Contact_ID;
    uint16_t Emergency_ID;
    uint64_t Primary_ID; 
    uint64_t Vital_ID;

    //Sets primary key to be the Patient_ID
    uint64_t  primary_key() const {
      return Patient_ID;
    }

    //Sets secondary key to be the name
    uint64_t secondary_key() const {
      return Name.value;
    }
  };
    /*_n is the name of the table
  * takes in a name value
  */
  typedef eosio::multi_index<"patients"_n, patients,  
      indexed_by<"seckey"_n, const_mem_fun<patients, uint64_t, &patients::secondary_key>>> patients_index;


  //Contact Information
  struct [[eosio::table]] contact {
    uint64_t Contact_ID;
    std::string Address;
    std::string Phone;
    std::string Email;

    //Sets primary key to be the Contact_ID
    uint64_t  primary_key() const {
      return Contact_ID;
    }
    
  };
    typedef eosio::multi_index<"contact"_n, contact> contact_index;

  //Emergency Information
  struct [[eosio::table]] emergency {
    uint64_t Emergency_ID;
    std::string Name;
    std::string Relationship;
    uint64_t Contact_ID;

    //Sets primary key to be the Emergency_ID
    uint64_t  primary_key() const {
      return Emergency_ID;
    }
    
  };
    typedef eosio::multi_index<"emergency"_n, emergency> emergency_index;

  //Medical Professional Information
  struct [[eosio::table]] medical {
    uint64_t Admin_ID;
    name Name;
    std::string Hours;
    uint64_t Contact_ID;

    //Sets primary key to be the Admin_ID
    uint64_t  primary_key() const {
      return Admin_ID;
    }
    
    //Sets secondary key to be the Name
    uint64_t secondary_key() const {
      return Name.value;
    }
  };
    typedef eosio::multi_index<"medical"_n, medical,  
      indexed_by<"seckey"_n, const_mem_fun<medical, uint64_t, &medical::secondary_key>>> medical_index;

  //Patient Vital Information
  struct [[eosio::table]] vital {
    uint64_t Vital_ID;
    double BodyTemp;
    double PulseRate;
    double RespirationRate;
    double BloodPressure;
    uint64_t LastModified;

    //Sets primary key to be the Vital_ID
    uint64_t  primary_key() const {
      return Vital_ID;
    }
    
  };
    typedef eosio::multi_index<"vital"_n, vital> vital_index;


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

  //Inserts new contact
  //Only called within insertPatient() and addem()
  //User = Medical pro (Admin)
  [[eosio::action]] //Needed for ABI generation
  uint64_t addcontact(name user, 
                  std::string address, 
                  std::string phone,
                  std::string email) {
      //Ensures the account executing transaction has proper permissions
      require_auth(user);
      
      contact_index addressbook(get_self(), get_first_receiver().value);

      uint64_t res_ID = 0;;
      addressbook.emplace(user, [&](auto& row) {
          row.Contact_ID = addressbook.available_primary_key();
          row.Address = address;
          row.Phone = phone;
          res_ID = row.Contact_ID;
      });
      print("Contact is created on: ", now());
      send_summary(user, " successfully inserted contact");
    return res_ID;
  }

  //Inserts new emergency contact
  //User = Medical pro (Admin)
  [[eosio::action]] //Needed for ABI generation
  uint64_t addem(name user, 
                            std::string emergency, 
                            std::string relationship, 
                            std::string e_address, 
                            std::string e_phone, 
                            std::string e_email) {
      //Ensures the account executing transaction has proper permissions
      require_auth(user);
      
      emergency_index emergencycard(get_self(), get_first_receiver().value);

      uint64_t res_ID = 0;;
      emergencycard.emplace(user, [&](auto& row) {
          row.Emergency_ID = emergencycard.available_primary_key();
          row.Name = emergency;
          row.Relationship = relationship;
          row.Contact_ID = addcontact(user, e_address, e_phone, e_email);
          res_ID = row.Emergency_ID;
      });
      print("Emergency contact is created on: ", now());
      send_summary(user, " successfully inserted emergency contact");
    return res_ID;
  }

  //Inserts new medical pro
  [[eosio::action]] //Needed for ABI generation
  uint64_t addstaff(name user, 
                            name primary, 
                            std::string hours, 
                            std::string p_address, 
                            std::string p_phone, 
                            std::string p_email) {
      //Ensures the account executing transaction has proper permissions
      require_auth(user);
      
      medical_index staff(get_self(), get_first_receiver().value);

      uint64_t res_ID = 0;;
      staff.emplace(user, [&](auto& row) {
          row.Admin_ID = staff.available_primary_key();
          row.Name = primary;
          row.Hours = hours;
          row.Contact_ID = addcontact(user, p_address, p_phone, p_email);
          res_ID = row.Admin_ID;
      });
      print("Primary contact is created on: ", now());
      send_summary(user, " successfully inserted primary contact");
    return res_ID;
  }

  //Inserts new vitals
  [[eosio::action]] //Needed for ABI generation
  uint64_t addvital(name user) {
      //Ensures the account executing transaction has proper permissions
      require_auth(user);
      
      vital_index monitor(get_self(), get_first_receiver().value);

      uint64_t res_ID = 0;
      monitor.emplace(user, [&](auto& row) {
          row.Vital_ID = monitor.available_primary_key();
          row.BodyTemp = 98.6; // Fahrenheit
          row.PulseRate = 60.0; // BPM
          row.RespirationRate = 12.0; //Breaths per minute
          row.BloodPressure = 120.0; //mm HG
          row.LastModified = now();
          res_ID = row.Vital_ID;
      });
      print("Vitals created on: ", now());
      send_summary(user, " successfully inserted vitals");
    return res_ID;
  }
  
  
  //Inserts new patient
  //Assumes new contact, emergency, and vital
  //User = Medical pro (Admin) and is already added
  [[eosio::action]] //Needed for ABI generation
  void addpatient(name user, 
                  name patient_name,
                  std::string gender, 
                  std::string dob, 
                  std::string address, 
                  std::string phone,
                  std::string email,
                  std::string emergency,
                  std::string relationship,
                  std::string e_address,
                  std::string e_phone,
                  std::string e_email) {
      //Ensures the account executing transaction has proper permissions
      require_auth(user);
      
      /* Creates Table to index from
      *  first param specifies the owner of this table
      *     in this case the name of the contract
      *  second param is the scope and ensures uniqueness of the table within the contract
      *     in this case the account name this contract is deployed to
      */
      patients_index hospital(get_self(), get_first_receiver().value);

      //Iterator for patient using patient_id as key
      uint64_t  key_int = patient_name.value;
      
      //Iterate with secondary key
      auto secpatient = hospital.get_index<name("seckey")>();
      
      auto iterator = secpatient.find(key_int);
      if( iterator == secpatient.end()) {
            hospital.emplace(user, [&](auto& row) {
                row.Patient_ID = hospital.available_primary_key();
                row.Name = patient_name;
                row.Gender = gender;
                row.DOB = dob;
                row.Contact_ID = addcontact(user, address, phone, email);
                row.Emergency_ID = addem(user, emergency, relationship, e_address, e_phone, e_email);
                row.Primary_ID = getMedicalID(user);
                row.Vital_ID = addvital(user);
            });
            print("Patient: ",patient_name, " is created on: ", now());
            send_summary(user, " successfully inserted patient");
        }
        else {
              //The patient is in the table
              print("ALREADY EXISTS! Patient: ", patient_name);
        }
  }

  //Erases Contact
  void erase_contact(name user, uint64_t contact_val) {
      contact_index contact(get_self(), get_first_receiver().value);
      contact.erase(contact.find(contact_val));
  }

  //Erases Contact
  void erase_emergency(name user, uint64_t emergency_val) {
      emergency_index emergency(get_self(), get_first_receiver().value);
      erase_contact(user, emergency.find(emergency_val)->Contact_ID);
      emergency.erase(emergency.find(emergency_val));
  }

  //Erases Vital
  void erase_vital(name user, uint64_t vital_val) {
      vital_index vital(get_self(), get_first_receiver().value);
      vital.erase(vital.find(vital_val));
  }

  //Erases patient
  //Will also erase corresponding contact, emergency and vitals
  [[eosio::action]]
  void delpatient(name user, name patient_name) {
    require_auth(user);

    patients_index patient(get_self(), get_first_receiver().value);

    //Iterator for patient using patient_id as key
    uint64_t  key_int = patient_name.value;
    
    //Iterate with secondary key
    auto secpatient = patient.get_index<name("seckey")>();
    
    auto iterator = secpatient.find(key_int);

    if( iterator == secpatient.end() ) {
          print("DOES NOT EXIST! Patient: ", patient_name);
      }
      else {
          erase_contact(user, iterator->Contact_ID);
          erase_emergency(user, iterator->Emergency_ID);
          erase_vital(user, iterator->Vital_ID);
          secpatient.erase(iterator);
          print("REMOVED Patient: ", patient_name, "  on: ", now());
          send_summary(user, " successfully removed patient");

      }
  }

  // //Erases parking spot 
  // [[eosio::action]]
  // void clear(name user) {
  //   require_auth(user);

  //   park_index patient(get_self(), get_first_receiver().value);
  //   auto it = patient.begin();
  //   while (it != patient.end()) {
  //       it = patient.erase(it);
  //   }
  // }
 
  //Returns the MedicalID of a user
  uint64_t getMedicalID(name user) {
    require_auth(user);

    medical_index staff(get_self(), get_first_receiver().value);

    //Iterator for parking spots using spot_id as key
    uint64_t  key_int = user.value; 

    //Iterate with secondary key
    auto secmedical = staff.get_index<name("seckey")>();
    auto iterator = secmedical.find(key_int);

    return iterator->Admin_ID;
  }

  //TODO Contact Update, Emergency Update
  //Erases patient
  //Will also erase corresponding contact, emergency and vitals
  [[eosio::action]]
  void modvital(name user, 
                         name patient_name,    
                         double newBodyTemp,
                         double newPulseRate,
                         double newRespirationRate,
                         double newBloodPressure) {
    require_auth(user);

    patients_index patient(get_self(), get_first_receiver().value);

    //Iterator for patient using patient_id as key
    uint64_t  key_int = patient_name.value;
    
    //Iterate with secondary key
    auto secpatient = patient.get_index<name("seckey")>();
    
    auto iterator = secpatient.find(key_int);

    if( iterator == secpatient.end() ) {
          print("DOES NOT EXIST! Patient: ", patient_name);
      }
      else {
          erase_contact(user, iterator->Contact_ID);
          erase_emergency(user, iterator->Emergency_ID);
          erase_vital(user, iterator->Vital_ID);
          secpatient.erase(iterator);
          print("UPDATED Patient Vitals: Body Temp: ", newBodyTemp, "\nPulse Rate: ", newPulseRate , "\nRespiration Rate: ", newRespirationRate, "\nBlood Pressure: ", newBloodPressure, "\non: ", now());
          send_summary(user, " successfully updated patient vitals");
      }
  }
};

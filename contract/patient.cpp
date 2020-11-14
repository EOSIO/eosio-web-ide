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

    //Sets secondary key to be the name
    uiint64_t secondary_key() const {
      return Name.value;
    }
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
    
    //Sets secondary key to be the Name
    uint64_t secondary_key() const {
      return Name.value;
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

  /*_n is the name of the table
  * takes in a name value
  */
  typedef eosio::multi_index<"patient"_n, patient,  
      indexed_by<"seckey"_n, const_mem_fun<patient, uint64_t, &patient::secondary_key>>> patient_index;
  
  /*_n is the name of the table
  * takes in a name value
  */
  typedef eosio::multi_index<"medical"_n, medical,  
      indexed_by<"seckey"_n, const_mem_fun<medical, uint64_t, &medical::secondary_key>>> medical_index;


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
  //Only called within insert_patient() and insert_emergency()
  //User = Medical pro (Admin)
  [[eosio::action]] //Needed for ABI generation
  uint64_t insert_contact(name user, 
                  string address, 
                  string phone,
                  string email) {
      //Ensures the account executing transaction has proper permissions
      require_auth(user);
      
      contact_id contact(get_self(), get_first_receiver().value);

      res_ID = 0
      contact.emplace(user, [&](auto& row) {
          row.Contact_ID = contact.available_primary_key();
          row.Address = address;
          row.Phone = phone;
          row.DOB = dob;
          res_ID = row.Contact_ID
      });
      print("Contact is created on: ", now());
      send_summary(user, " successfully inserted contact");
    return res_ID;
  }

  //Inserts new emergency contact
  //User = Medical pro (Admin)
  [[eosio::action]] //Needed for ABI generation
  uint64_t insert_emergency(name user, 
                            string emergency, 
                            string relationship, 
                            string e_address, 
                            string e_phone, 
                            string e_email) {
      //Ensures the account executing transaction has proper permissions
      require_auth(user);
      
      res_ID = 0
      emergency.emplace(user, [&](auto& row) {
          row.Emergency_ID = emergency.available_primary_key();
          row.Name = emergency;
          row.Relationship = relationship;
          row.Contact_ID = insert_contact(user, e_address, e_phone, e_email);
          res_ID = row.Emergency_ID
      });
      print("Emergency contact is created on: ", now());
      send_summary(user, " successfully inserted emergency contact");
    return res_ID;
  }

  //Inserts new medical pro
  [[eosio::action]] //Needed for ABI generation
  uint64_t insert_primary(name user, 
                            name primary, 
                            string hours, 
                            string p_address, 
                            string p_phone, 
                            string p_email) {
      //Ensures the account executing transaction has proper permissions
      require_auth(user);
      
      res_ID = 0
      medical.emplace(user, [&](auto& row) {
          row.Admin_ID = medical.available_primary_key();
          row.Name = primary;
          row.Hours = hours;
          row.Contact_ID = insert_contact(user, p_address, p_phone, p_email);
          res_ID = row.Admin_ID
      });
      print("Primary contact is created on: ", now());
      send_summary(user, " successfully inserted primary contact");
    return res_ID;
  }

  //Inserts new vitals
  [[eosio::action]] //Needed for ABI generation
  uint64_t insert_vitals(name user) {
      //Ensures the account executing transaction has proper permissions
      require_auth(user);
      
      res_ID = 0
      vital.emplace(user, [&](auto& row) {
          row.Vital_ID = vital.available_primary_key();
          row.BodyTemp = 98.6; // Fahrenheit
          row.PulseRate = 60.0; // BPM
          row.RespirationRate = 12.0; //Breaths per minute
          row.BloodPressure = 120.0; //mm HG
          row.LastModified = now();
          res_ID = row.Vital_ID
      });
      print("Vitals created on: ", now());
      send_summary(user, " successfully inserted vitals");
    return res_ID;
  }
  
  
  //Inserts new patient
  //Assumes new contact, emergency, and vital
  //User = Medical pro (Admin) and is already added
  [[eosio::action]] //Needed for ABI generation
  void insert_patient(name user, 
                  name patient,
                  string gender, 
                  string dob, 
                  string address, 
                  string phone,
                  string email,
                  string emergency,
                  string relationship,
                  string e_address,
                  string e_phone,
                  string e_email) {
      //Ensures the account executing transaction has proper permissions
      require_auth(user);
      
      /* Creates Table to index from
      *  first param specifies the owner of this table
      *     in this case the name of the contract
      *  second param is the scope and ensures uniqueness of the table within the contract
      *     in this case the account name this contract is deployed to
      */
      patient_index patient(get_self(), get_first_receiver().value);

      //Iterator for patient using patient_id as key
      uint64_t  key_int = name.value
      
      //Iterate with secondary key
      auto secpatient = patient.get_index<name("seckey")>();
      
      auto iterator = secpatient.find(key_int);
      if( iterator == secpatient.end()) {
            //The parking spot isn't in the table
            patient.emplace(user, [&](auto& row) {
                row.Patient_ID = patient.available_primary_key();
                row.Name = patient;
                row.Gender = gender;
                row.DOB = dob;
                row.Contact_ID = insert_contact(user, address, phone, email);
                row.Emergency_ID = insert_emergency(user, emergency, relationship, e_address, e_phone, e_email);
                row.Primary_ID = getMedicalID(user);
                row.Vital_ID = insert_vitals(user);
            });
            print("Patient: ",patient" is created on: ", now());
            send_summary(user, " successfully inserted patient");
        }
        else {
              //The patient is in the table
              print("ALREADY EXISTS! Patient: ", patient);
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
      erase_contact(user, emergency_val->Contact_ID);
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
  void erase_patient(name user, name patient) {
    require_auth(user);

    patient_index patient(get_self(), get_first_receiver().value);

    //Iterator for patient using patient_id as key
    uint64_t  key_int = name.value
    
    //Iterate with secondary key
    auto secpatient = patient.get_index<name("seckey")>();
    
    auto iterator = secpatient.find(key_int);

    if( iterator == secpatient.end() ) {
          print("DOES NOT EXIST! Patient: ", patient);
      }
      else {
          erase_contact(user, iterator->Contact_ID);
          erase_emergency(user, iterator->Emergency_ID);
          erase_vital(user, iterator->Vital_ID)
          secpatient.erase(iterator);
          print("REMOVED Patient: ", patient, "  on: ", now());
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

    medical_index medical(get_self(), get_first_receiver().value);

    //Iterator for parking spots using spot_id as key
    uint64_t  key_int = name.value; 

    //Iterate with secondary key
    auto secmedical = medical.get_index<name("seckey")>();
    auto iterator = secmedical.find(key_int);

    return iterator->Admin_ID;
  }

  //TODO Contact Update, Emergency Update
  //Erases patient
  //Will also erase corresponding contact, emergency and vitals
  [[eosio::action]]
  void mod_patient_vital(name user, 
                         name patient,    
                         float64 newBodyTemp,
                         float64 newPulseRate,
                         float64 newRespirationRate,
                         float64 newBloodPressure) {
    require_auth(user);

    patient_index patient(get_self(), get_first_receiver().value);

    //Iterator for patient using patient_id as key
    uint64_t  key_int = name.value
    
    //Iterate with secondary key
    auto secpatient = patient.get_index<name("seckey")>();
    
    auto iterator = secpatient.find(key_int);

    if( iterator == secpatient.end() ) {
          print("DOES NOT EXIST! Patient: ", patient);
      }
      else {
          erase_contact(user, iterator->Contact_ID);
          erase_emergency(user, iterator->Emergency_ID);
          erase_vital(user, iterator->Vital_ID)
          secpatient.erase(iterator);
          print("UPDATED Patient Vitals: Body Temp: ", newBodyTemp, "\nPulse Rate: ", newPulseRate , "\nRespiration Rate: ", newRespirationRate, "\nBlood Pressure: ", newBloodPressure, "\non: ", now());
          send_summary(user, " successfully updated patient vitals");
      }
  }
};

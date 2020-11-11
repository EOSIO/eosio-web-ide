CREATE TABLE [Vitals] (
  [Vital_ID] int,
  [BodyTemp] float,
  [PulseRate] float,
  [RespirationRate] float,
  [BloodPressure] float,
  [LastModified] date,
  PRIMARY KEY ([Vital_ID])
);

CREATE TABLE [MedicalPro] (
  [Admin_ID] int,
  [Name] varchar(50),
  [Hours] varchar(50),
  [Contact_ID] int,
  PRIMARY KEY ([Admin_ID])
);

CREATE TABLE [SignIn] (
  [Sign_ID] int,
  [Patient_ID] int,
  [Location_ID] int,
  [Sign_Date] datetime,
  PRIMARY KEY ([Sign_ID])
);

CREATE TABLE [ContactInfo] (
  [Contact_ID] Type,
  [Address] varchar(50),
  [Phone] varchar(10),
  [Email] varchar(50),
  PRIMARY KEY ([Contact_ID])
);

CREATE TABLE [Location] (
  [Location_ID] int,
  [Name] varchar(50),
  [Room] varchar(50),
  [Address] varchar(50),
  [Manager] int,
  [CurrentOccupancy] int,
  [Limit] int,
  [Assignment] datetime,
  [Patient_ID] int,
  PRIMARY KEY ([Location_ID])
);

CREATE TABLE [Emergency] (
  [Emergency_ID] int,
  [Name] varchar(50),
  [Relationship] varchar(50),
  [Contact_ID] int,
  PRIMARY KEY ([Emergency_ID])
);

CREATE TABLE [Patient] (
  [Patient_ID] int,
  [Name] varchar(50),
  [Gender] varchar(50),
  [DOB] date,
  [Contact_ID] int,
  [Emergency_ID] int,
  [Primary_ID] int,
  [Vital_ID] int,
  PRIMARY KEY ([Patient_ID])
);



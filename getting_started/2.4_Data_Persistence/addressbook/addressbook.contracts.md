<h1 class="contract">upsert</h1>
---
spec-version: 0.0.2
title: Upsert
summary: This action will either insert or update an entry in the address book. If an entry exists with the same name as the specified user parameter, the record is updated with the first_name, last_name, street, city, and state parameters. If a record does not exist, a new record is created. The data is stored in the multi index table. The ram costs are paid by the smart contract.
icon: 

<h1 class="contract">erase</h1>
---
spec-version: 0.0.2
title: Erase
summary: This action will remove an entry from the address book if an entry in the multi index table exists with the specified name. 
icon: 
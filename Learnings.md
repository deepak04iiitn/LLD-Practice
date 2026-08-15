1. Whenever in any problem we see the requirement that user can register and login, then instead  of giving both of these reponsibilities to User itself, create a separate Authentication Service for auth actions.

2. Wherever we see that user can search or browse by multiple criterias, then don't go on creating multiple methods for each criteria, instead create one SearchCriteria Class
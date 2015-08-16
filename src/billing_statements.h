#pragma once
  typedef struct {
  int amount;
  int month;
  int day;
  int year;
  
  char payee[32];
  char amt[32];
  char title[32];
  char mnth[32];
  char dy[32];
  char yr[32];
  
  char fulldate[32];
} Bill;

void billing_start();
void billing_end();
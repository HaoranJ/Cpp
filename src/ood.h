#ifndef OOD_H
#define OOD_H

class MyDate
{
  public:
  MyDate(int d, int m) {
    this->day = d;
    this->month = m;
  };
  void Day(int d);
  int Day() const;
  int day;
  int month;
  static int counter;
};

#endif
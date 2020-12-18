#include <iostream>
#include <string>

using namespace std;


int main ()
{
  int  startandend[2];
  for (int i=0; i<2; i++){cin>>startandend[i];}
  int  time[5000];
  for (int xx = 0; xx < 5000; xx++){time[xx] = -1;}
  string name[5000];
  int i = 0;
  while(cin>>time[i]>>name[i]){i = i + 1;}
  string timeline[1000][5] = {"kong"};
  const int maxhour = (startandend[1] - startandend[0])/3600;

  for (int wai = 0; wai < 1000; wai++)
  {
      for (int nei = 0; nei < 5; nei++){timeline[wai][nei] = "kong";}
  }



  for (int i = 0; i<5000; i++)
  {
      int hour = ((time[i]- startandend[0])/3600) + 1 ;
      int counter = 0;
      bool flag = false;
      while((flag == false) && (time[i] >= 0))
      {
          if (timeline[hour][counter] == "kong")
          {
              timeline[hour][counter] = name[i];
              flag = true;
          }
          else
          {
              if (counter > 4){hour++; counter = 0;}
              else{counter++;}
          }
      }
  }
  //---------------judge start-------------------
  int re = 0;
  for (int judge=999; judge >= 1; judge--)
  {
      if ((timeline[judge][4] != "kong")||(timeline[judge][0] != "kong")){re = judge;break;}
  }
  int maxer = min(maxhour,re);
  //---------------print-----------------------
  for (int mm = 1; mm < maxer+1; mm++)
  {
      cout << "Hour" << mm << ":" << endl;
      for(int j = 0; j<5; j++)
      {
          if (timeline[mm][j] != "kong"){cout << timeline[mm][j]<<" ";}
      }
      cout << "" <<endl;
  }
  //_________________________________________________
  if(re > maxhour)
    {cout << "Remaining files:"<<endl ;
    for (int mm = maxhour+1 ; mm < 1000; mm++)
    {   if (timeline[mm][0] != "kong")
        {for(int jj = 0; jj<5; jj++)
        {
            if (timeline[mm][jj] != "kong"){cout << timeline[mm][jj]<<" ";}
        }
        }
    }
    }

  return 0;
}

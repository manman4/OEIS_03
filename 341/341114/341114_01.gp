\\ 17843はあり

forprime(x=2, 2, for(y=12, 12, a = contfrac(x^(1/y)); for(k=2, 20, print1(contfracpnqn(a[1..k]),", ")) ));

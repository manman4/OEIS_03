a(n)=my(s=""); for(k=1, n, s=Str(s, (11-2*k)%10)); eval(s);
cnt=0;
for(k=1, 2000, if(isprime(a(k)), print(cnt+=1, " ", a(k))))

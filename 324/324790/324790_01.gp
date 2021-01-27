s=3;pr=3;
for(k=2,10^11,while(!isprime(pr),pr+=2);if(s<pr,s+=pr,s-=pr);pr+=2;if(s==0,print(k)))

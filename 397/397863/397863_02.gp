a_vector(n,s,t)=my(v=vector(n+1)); v[1]=1; for(i=1,n, v[i+1]=sum(k=0,i-1, binomial(t+s*k, i-k-1)*v[k+1])); v;
A_rec(n,s,t)=my(v=a_vector(n,s,t)); sum(i=0,n, v[i+1]*x^i)+O(x^(n+1));

A_def(n,s,t)=my(X=x+O(x^(n+1)), A=1+O(x^(n+1))); for(r=1,n+1, A=1+X*(1+X)^t*subst(A,x,X*(1+X)^s)); A;

check(n,s,t)=if(A_rec(n,s,t)==A_def(n,s,t), "ok", "fail");

check(20,3,2)

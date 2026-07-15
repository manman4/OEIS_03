a_by_recurrence(n, s, t) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=i*sum(k=0, i-1, (s*k+t)^(i-k-1)*binomial(i-1, k)*v[k+1])); v;
A_by_recurrence_laplace(n, s, t) = my(v=a_by_recurrence(n, s, t)); sum(i=0,n, v[i+1]*x^i)+O(x^(n+1));

A_by_definition_laplace(n, s, t)=my(X=x+O(x^(n+1)), A=1+O(x^(n+1))); for(r=1, n+1, A=1+X*exp(t*X)*subst(A,x,X*exp(s*X))); serlaplace(A);

check_exp(n,s,t) = if(A_by_recurrence_laplace(n, s, t)==A_by_definition_laplace(n, s, t), "ok", "fail");

check_exp(20, 1, 1)

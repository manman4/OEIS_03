def A(n)
  a = []
  (1..n).to_a.permutation{|i|
    flag = true
    m = -1
    (1..n - 1).each{|j| 
      s = i[j - 1] % i[j]
      if s > m
        m = s
      else
        flag = false
        break
      end
    }
    a << i if flag
  }
  a
end

p (1..14).map{|i| p A(i).size}
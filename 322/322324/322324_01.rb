require 'prime'

def A(k, n)
  s = 1
  (1..n).each{|i|
    if n % i == 0
      if i.prime?
        s *= 1 - i ** k
      end
    end
  }
  s
end

def B(n)
  ary = []
  (1..n).each{|i|
    (1..i).each{|j|
      ary << A(i - j, j)
    }
  }
  ary
end

n = 140
ary = B(n)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

require 'prime'

def A(n)
  s = 1
  (1..n).each{|i|
    if n % i == 0
      if i.prime?
        s *= 1 - i ** i
      end
    end
  }
  s
end

n = 500
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

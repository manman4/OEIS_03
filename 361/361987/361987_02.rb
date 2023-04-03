require 'prime'

def A(n)
  return 1 if n == 1
  s = 1
  n.prime_division.each{|i, j|
    if i == 2
      s *= 2 ** (3 * j - 1)
    else
      if j == 1
        s *= -i * i
      else
        s *= 0
      end
    end
  }
  s
end

n = 10000
(1..n).each{|i| 
  print i
  print ' '
  puts A(i)
}


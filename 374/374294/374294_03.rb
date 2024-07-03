require 'prime'

def find_factors(n)
  a = []
  (2..n - 1).each{|i|
    a << [i] if n % i == 0
  }
  ary = [[n]]
  while a.size > 0
    b = []
    a.each{|x|
      prod_x = x.inject(:*)
      (x[-1]..n / prod_x).each{|y|
        if n == prod_x * y
          ary << x + [y]
        elsif n % (prod_x * y) == 0
          b << x + [y]
        end
      }
    }
    a = b
  end
  ary
end

prime_ary = Prime.take(1000).select{|i| i % 8 == 1 || i % 8 == 3}
p prime_ary

a = [1]
(2..45).each{|i|
  min = 3 ** i
  find_factors(i).each{|ary|
     s = 1
    (0..ary.size - 1).each{|j|
      s *= prime_ary[j] ** (ary[-1 - j] - 1)
    }
    min = s if s < min
  }
  a << min
}
p a




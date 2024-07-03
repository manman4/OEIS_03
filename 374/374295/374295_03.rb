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

prime_ary = Prime.take(1000).select{|i| i % 3 == 1}
p prime_ary

N = 90

a = [0, 1]
(2..N).each{|i|
  min = 7 ** i
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

b = (0..N).map{|i| a[i]}
(0..N / 3).each{|i|
  b[3 * i] = [a[3 * i], 4 * a[i]].min}
p b




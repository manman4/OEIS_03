class Counter
  @@instances = 0
  def initialize
    @@instances += 1
  end
  def self.instances
    @@instances
  end
  def self.zero
    @@instances = 0
  end
end

def search(a, num, d, k, n)
  if num == 0
    Counter.new
  else
    (k * n - k + 1).times{|i|
      if a[i] == 0
        (i + d + 1..k * n - k + 1).each{|j|
          if (k - 1) * j - (k - 2) * i < k * n
            if (1..k - 1).all?{|m| a[m * j - (m - 1) * i] == 0}
              (0..k - 1).all?{|m| a[m * j - (m - 1) * i] = num}
              search(a, num - 1, j - i - 1, k, n)
              (0..k - 1).all?{|m| a[m * j - (m - 1) * i] = 0}
            end
          end
        }
      end
    }
  end
end

def a32(k, n)
  a = [0] * n * k
  Counter.zero
  search(a, n, 0, k, n)
  Counter.instances
end

p (2..9).map{|i| p a32(5, i)}
#p (2..10).map{|i| p a32(i, i - 1)}

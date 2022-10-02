class Count
  @@cnt = 0
  def initialize
    @@cnt += 1
  end
  def self.instances
    @@cnt
  end
  def self.to_zero
    @@cnt = 0
  end
end

def search(a, num, n)
  if num == n + 1
    # p [n, a]
    Count.new
  else
    ((0..n).to_a - a[0..num - 1]).each{|i|
      flag = true
      (1..num - 1).each{|j|
        if num % j == 0
          flag = false if i % a[j] > 0
        end
      }
      if flag
        a[num] = i
        search(a, num + 1, n)
        a[num] = 0
      end
    }
  end
end

def a357328(n)
  a = [0] * (n + 1)
  Count.to_zero
  search(a, 1, n)
  Count.instances
end

p (1..30).map{|i| p a357328(i)}

# [1, 1, 2, 1, 2, 1, 2, 2, 2, 1, 2, 2, 6, 4, 2, 2, 6, 6, 24, 24, 24, 6, 24, 24, 24, 12, 12, 12, 48, 48]
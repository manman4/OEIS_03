def avoids_132?(perm)
  third = 0
  stack = []

  perm.reverse_each{|x|
    return false if x < third

    while !stack.empty? && stack[-1] < x
      third = stack.pop
    end
    stack << x
  }
  true
end

def order_divides_3?(perm)
  n = perm.length
  used = Array.new(n, false)

  0.upto(n - 1){|start|
    next if used[start]

    len = 0
    i = start
    until used[i]
      used[i] = true
      i = perm[i] - 1
      len += 1
      return false if len > 3
    end
    return false unless len == 1 || len == 3
  }
  true
end

def append_max(perm)
  perm + [perm.length + 1]
end

def build_templates(limit)
  templates = Array.new(limit + 1){[]}

  3.upto(limit){|n|
    ary = []
    2.upto(n - 1){|s2|
      (s2 + 1).upto(n){|s3|
        keep = []
        1.upto(n){|v|
          next if v == 1 || v == s2 || v == s3
          keep << v
        }
        ary << [keep, s2, s3, 0]
        ary << [keep, s2, s3, 1]
      }
    }
    templates[n] = ary
  }

  templates
end

def lift_insert(perm, keep, s2, s3, orient)
  n = perm.length + 3
  lifted = Array.new(n + 1, 0)

  0.upto(perm.length - 1){|i|
    lifted[keep[i]] = keep[perm[i] - 1]
  }

  if orient == 0
    lifted[1] = s2
    lifted[s2] = s3
    lifted[s3] = 1
  else
    lifted[1] = s3
    lifted[s3] = s2
    lifted[s2] = 1
  end

  lifted[1..]
end

def generate_good_perms(limit, verify: false)
  raise ArgumentError, "limit must be nonnegative" if limit < 0

  templates = build_templates(limit)
  good = Array.new(limit + 1){[]}
  good[0] = [[]]

  1.upto(limit) do |n|
    list = []
    seen = {}

    good[n - 1].each{|perm|
      child = append_max(perm)
      next if seen.key?(child)
      seen[child] = true
      list << child
    }

    if n >= 3
      good[n - 3].each{|perm|
        templates[n].each{|keep, s2, s3, orient|
          child = lift_insert(perm, keep, s2, s3, orient)
          next unless avoids_132?(child)
          next if seen.key?(child)

          if verify
            raise "order check failed" unless order_divides_3?(child)
          end

          seen[child] = true
          list << child
        }
      }
    end

    if verify
      list.each{|perm|
        raise "132 check failed" unless avoids_132?(perm)
        raise "order check failed" unless order_divides_3?(perm)
      }
    end

    good[n] = list
  end

  good
end

def sequence(limit, verify: false)
  good = generate_good_perms(limit, verify: verify)
  good.map(&:length)
end

if __FILE__ == $PROGRAM_NAME
  limit = (ARGV[0] || 15).to_i
  verify = ARGV.include?("--verify")
  ary = sequence(limit, verify: verify)
  puts ary.join(", ")
  ary.each_with_index{|v, n|
    puts "#{n} #{v}"
  }
end

// Includes

#include "gjk.hh"
#include "algebra/vector2.hh"
#include "sys/msys_libc.h"

// local macros

#define EPA_DIST(X) X.s1[3].x



namespace phy
{


  // Local declarations

  namespace
  {

    // local constants
    const float epsilon = 0.0001f;

    // temporary data
    struct gjk_data
    {
      public:
        gjk_data()
          : size(0)
        {
        }

        vector3f vector;
        point3f s1[4];
        point3f s2[4];
        point3f sm[4];
        point3f p1;
        point3f p2;
        point3f pm;
        int size;
    };

    // support determination
    point3f support(Mesh* m, const vector3f& v)
    {
      point3f res = m->particles[0].position();
      float max = dot(res, v);

      for (int i = 1; i < m->particles.size; ++i)
      {
        const point3f& p = m->particles[i].position();
        float tmp = dot(p, v);

        if (tmp > max)
        {
          max = tmp;
          res = p;
        }
      }

      return res;
    }

    // projection
    inline float project(const vector3f & v0, const vector3f & v)
    {
      return dot(v, v0) / dot(v0, v0);
    }

    // projection of 0p on the triangle defined by v0 and v1
    inline vector2f project(const vector3f & v0, const vector3f & v1, const point3f & p)
    {
      vector2f res;

      const float v0v0 = dot(v0, v0);
      const float v0v1 = dot(v0, v1);
      const float v1v1 = dot(v1, v1);
      const float v0p  = dot(v0, p);
      const float v1p  = dot(v1, p);

      /* k0 */ res.x = (v0v1 * v1p - v1v1 * v0p) / (v0v0 * v1v1 - v0v1 * v0v1);
      /* k1 */ res.y = (v0v1 * v0p - v0v0 * v1p) / (v0v0 * v1v1 - v0v1 * v0v1);

      return res;
    }

    // 0-simplex data update
    void data1(gjk_data& d)
    {
      d.vector = d.pm;
      d.s1[0] = d.p1;
      d.s2[0] = d.p2;
      d.sm[0] = d.pm;
      d.size = 1;

      if (dot(d.vector, d.vector) <= epsilon)
        d.vector.x = 1.f;
    }

    // 1-simplex data update
    void data2(gjk_data& d, int np0, const vector3f& v)
    {
      d.vector = v;
      d.s1[0] = d.s1[np0];
      d.s2[0] = d.s2[np0];
      d.sm[0] = d.sm[np0];
      d.s1[1] = d.p1;
      d.s2[1] = d.p2;
      d.sm[1] = d.pm;
      d.size = 2;

      if (dot(d.vector, d.vector) <= epsilon)
      {
        d.vector = d.sm[1] - d.sm[0];
        d.vector = cross(d.vector, vector3f(d.vector.y, d.vector.z, d.vector.x));
      }
    }

    // 2-simplex data update
    void data3(gjk_data& d, int np0, int np1, const vector3f& v)
    {
      d.vector = v;
      d.s1[0] = d.s1[np0];
      d.s2[0] = d.s2[np0];
      d.sm[0] = d.sm[np0];
      d.s1[1] = d.s1[np1];
      d.s2[1] = d.s2[np1];
      d.sm[1] = d.sm[np1];
      d.s1[2] = d.p1;
      d.s2[2] = d.p2;
      d.sm[2] = d.pm;
      d.size = 3;

      if (dot(d.vector, d.vector) <= epsilon)
        d.vector = cross(d.sm[1] - d.sm[0], d.sm[2] - d.sm[0]);
    }

    // result
    void data4(gjk_data& d)
    {
      d.s1[3] = d.p1;
      d.s2[3] = d.p2;
      d.sm[3] = d.pm;
      d.size = 4;
    }

    // 0-simplex iteration
    void simp1(gjk_data& d)
    {
      assert(d.size == 1);

      vector3f v0 = d.sm[0] - d.pm;
      float k0 = project(v0, -d.pm);
      point3f p0 = d.pm + v0 * k0;

      // DBG("[gjk] 1: v0: (%f, %f, %f) len: %f", v0.x, v0.y, v0.z, norm(v0));
      // DBG("[gjk] 1: p0: (%f, %f, %f) len: %f", p0.x, p0.y, p0.z, norm(p0));
      // DBG("[gjk] 1: k0: %f", k0);
      assert(msys_fabsf(dot(v0, p0)) < 0.001);

      if (k0 < 0.f)
        data1(d);
      else
        data2(d, 0, p0);
    }

    // 1-simplex iteration
    void simp2(gjk_data& d)
    {
      assert(d.size == 2);

      vector3f v0 = d.sm[0] - d.pm;
      vector3f v1 = d.sm[1] - d.pm;
      float k0 = project(v0, -d.pm);
      float k1 = project(v1, -d.pm);
      vector2f k01 = project(v0, v1, d.pm);
      point3f p0 = d.pm + (v0 * k0);
      point3f p1 = d.pm + (v1 * k1);
      point3f p01 = d.pm + v0 * k01.x + v1 * k01.y;

      bool f0 = dot(p01 - p0, p1 - p0) < 0.f;
      bool f1 = dot(p01 - p1, p0 - p1) < 0.f;

      // float kp0 = ((v0 * k0) * v1) * (v1 * v1);
      // float kp1 = ((v1 * k1) * v0) * (v0 * v0);
      // point3f pp0 = d.pm + (v1 * kp0);
      // point3f pp1 = d.pm + (v0 * kp1);
      // bool f0 = ((pp0 - p0) * (-p0)) < 0.f;
      // bool f1 = ((pp1 - p1) * (-p1)) < 0.f;
      // point3f p01 = p0 + (pp0 - p0) * (((pp0 - p0) * (-p0)) / ((pp0 - p0) * (pp0 - p0)));

      // float nk0 = (v0 * v1) / (v1 * v1);
      // float nk1 = (v0 * v1) / (v0 * v0);
      // vector3f nv0 = (d.pm + v0) - (d.pm + v1 * nk0);
      // vector3f nv1 = (d.pm + v1) - (d.pm + v0 * nk1);
      // float prout0 = ((-p0) * nv1) / (nv1 * nv1);
      // float prout1 = ((-p1) * nv0) / (nv0 * nv0);

      // point3f pp0 = p0 + nv1 * prout0;
      // point3f pp1 = p1 + nv0 * prout1;

      // ("TEMP: %f, %f", (v0 * p0), (v0.x * p0.x + v0.y * p0.y + v0.z * p0.z));

      DBG("[gjk] 2: v0:  (%f, %f, %f) len: %f",  v0.x,  v0.y,  v0.z, norm(v0));
      DBG("[gjk] 2: v1:  (%f, %f, %f) len: %f",  v1.x,  v1.y,  v1.z, norm(v1));
      DBG("[gjk] 2: p0:  (%f, %f, %f) len: %f",  p0.x,  p0.y,  p0.z, norm(p0));
      DBG("[gjk] 2: p1:  (%f, %f, %f) len: %f",  p1.x,  p1.y,  p1.z, norm(p1));
      DBG("[gjk] 2: p01: (%f, %f, %f) len: %f", p01.x, p01.y, p01.z, norm(p01));
      DBG("[gjk] 2: f0:  %s", (f0 ? "true" : "false"));
      DBG("[gjk] 2: f1:  %s", (f1 ? "true" : "false"));
      DBG("[gjk] 2: k0:  %f", k0);
      DBG("[gjk] 2: k1:  %f", k1);
      DBG("[gjk] 2: k01: %f, %f", k01.x, k01.y);

      assert(msys_fabsf(dot(v0, p0)) < epsilon);
      assert(msys_fabsf(dot(v1, p1)) < epsilon);

      if (k0 < 0.f && k1 < 0.f)
        data1(d);
      else if (f0)
        data2(d, 0, p0);
      else if (f1)
        data2(d, 1, p1);
      else
      {
        assert(msys_fabsf(dot(v0, p01)) < epsilon);
        assert(msys_fabsf(dot(v1, p01)) < epsilon);
        data3(d, 0, 1, p01);
      }
    }

    // 2-simplex iteration
    bool simp3(gjk_data& d)
    {
      assert(d.size == 3);

      vector3f v0 = d.sm[0] - d.pm;
      vector3f v1 = d.sm[1] - d.pm;
      vector3f v2 = d.sm[2] - d.pm;
      float k0 = dot(v0, -d.pm) / dot(v0, v0);
      float k1 = dot(v1, -d.pm) / dot(v1, v1);
      float k2 = dot(v2, -d.pm) / dot(v2, v2);
      point3f p0  = d.pm + v0 * k0;
      point3f p1  = d.pm + v1 * k1;
      point3f p2  = d.pm + v2 * k2;
      vector2f k01 = project(v0, v1, d.pm);
      vector2f k02 = project(v0, v2, d.pm);
      vector2f k12 = project(v1, v2, d.pm);
      point3f p01 = d.pm + v0 * k01.x + v1 * k01.y;
      point3f p02 = d.pm + v0 * k02.x + v2 * k02.y;
      point3f p12 = d.pm + v1 * k12.x + v2 * k12.y;

      bool f01 = dot(-p01, p2 - p01) < 0.f;
      bool f02 = dot(-p02, p1 - p02) < 0.f;
      bool f12 = dot(-p12, p0 - p12) < 0.f;

      DBG("[gjk] 3: v0:  (%f, %f, %f) len: %f",  v0.x,  v0.y,  v0.z, norm(v0));
      DBG("[gjk] 3: v1:  (%f, %f, %f) len: %f",  v1.x,  v1.y,  v1.z, norm(v1));
      DBG("[gjk] 3: v2:  (%f, %f, %f) len: %f",  v2.x,  v2.y,  v2.z, norm(v2));
      DBG("[gjk] 3: p0:  (%f, %f, %f) len: %f",  p0.x,  p0.y,  p0.z, norm(p0));
      DBG("[gjk] 3: p1:  (%f, %f, %f) len: %f",  p1.x,  p1.y,  p1.z, norm(p1));
      DBG("[gjk] 3: p2:  (%f, %f, %f) len: %f",  p2.x,  p2.y,  p2.z, norm(p2));
      DBG("[gjk] 3: p01: (%f, %f, %f) len: %f", p01.x, p01.y, p01.z, norm(p01));
      DBG("[gjk] 3: p02: (%f, %f, %f) len: %f", p02.x, p02.y, p02.z, norm(p02));
      DBG("[gjk] 3: p12: (%f, %f, %f) len: %f", p12.x, p12.y, p12.z, norm(p12));
      DBG("[gjk] 3: f01: %s", (f01 ? "true" : "false"));
      DBG("[gjk] 3: f02: %s", (f02 ? "true" : "false"));
      DBG("[gjk] 3: f12: %s", (f12 ? "true" : "false"));
      DBG("[gjk] 3: k0:  %f", k0);
      DBG("[gjk] 3: k1:  %f", k1);
      DBG("[gjk] 3: k2:  %f", k2);
      DBG("[gjk] 2: k01: %f, %f", k01.x, k01.y);
      DBG("[gjk] 2: k02: %f, %f", k02.x, k02.y);
      DBG("[gjk] 2: k12: %f, %f", k12.x, k12.y);
      assert(msys_fabsf(dot(v0, p0)) < epsilon);
      assert(msys_fabsf(dot(v1, p1)) < epsilon);
      assert(msys_fabsf(dot(v2, p2)) < epsilon);

      if (k0 < 0.f && k1 < 0.f && k2 < 0.f)
        data1(d);
      else if (f01 && f02)
        data2(d, 0, p0);
      else if (f01 && f12)
        data2(d, 1, p1);
      else if (f02 && f12)
        data2(d, 2, p2);
      else if (f01)
      {
        assert(msys_fabsf(dot(v0, p01)) < epsilon);
        assert(msys_fabsf(dot(v1, p01)) < epsilon);
        data3(d, 0, 1, p01);
      }
      else if (f02)
      {
        assert(msys_fabsf(dot(v0, p02)) < epsilon);
        assert(msys_fabsf(dot(v2, p02)) < epsilon);
        data3(d, 0, 2, p02);
      }
      else if (f12)
      {
        assert(msys_fabsf(dot(v1, p12)) < epsilon);
        assert(msys_fabsf(dot(v2, p12)) < epsilon);
        data3(d, 1, 2, p12);
      }
      else
      {
        data4(d);
        return true;
      }

      return false;
    }


    // EPA helpers
    bool epa_insert(Array<gjk_data>& s, gjk_data& nd)
    {
      vector3f v1 = nd.sm[1] - nd.sm[0];
      vector3f v2 = nd.sm[2] - nd.sm[0];
      float k1 = project(v1, -nd.sm[0]);
      float k2 = project(v2, -nd.sm[0]);
      vector2f k12 = project(v1, v2, nd.sm[0]);
      point3f p01 = nd.sm[0] + v1 * k1;
      point3f p02 = nd.sm[0] + v2 * k2;
      point3f p12 = nd.sm[0] + v1 * k12.x + v2 * k12.y;
      bool valid = k12.x >= 0.f && k12.y >= 0.f && k12.x + k12.y <= 1.f;

      DBG("[epa] p0:  (%f, %f, %f)", nd.sm[0].x, nd.sm[0].y, nd.sm[0].z);
      DBG("[epa] p1:  (%f, %f, %f)", nd.sm[1].x, nd.sm[1].y, nd.sm[1].z);
      DBG("[epa] p2:  (%f, %f, %f)", nd.sm[2].x, nd.sm[2].y, nd.sm[2].z);
      DBG("[epa] p01: (%f, %f, %f)",      p01.x,      p01.y,      p01.z);
      DBG("[epa] p02: (%f, %f, %f)",      p02.x,      p02.y,      p02.z);
      DBG("[epa] p12: (%f, %f, %f)",      p12.x,      p12.y,      p12.z);
      DBG("[epa] v1:  (%f, %f, %f) len: %f",  v1.x,  v1.y,  v1.z, norm(v1));
      DBG("[epa] v2:  (%f, %f, %f) len: %f",  v2.x,  v2.y,  v2.z, norm(v2));
      DBG("[epa] k1:  %f", k1);
      DBG("[epa] k2:  %f", k2);
      DBG("[epa] k12: %f, %f", k12.x, k12.y);
      DBG("[epa] => %s", (valid ? "valid" : "invalid"));

      if (!valid)
        return false;

      nd.p1 = nd.s1[0]
        + (nd.s1[1] - nd.s1[0]) * k12.x
        + (nd.s1[2] - nd.s1[0]) * k12.y;
      nd.p2 = nd.s2[0]
        + (nd.s2[1] - nd.s2[0]) * k12.x
        + (nd.s2[2] - nd.s2[0]) * k12.y;
      nd.pm = p12;
      nd.vector = p12;
      EPA_DIST(nd) = dot(nd.pm, nd.pm);

      if (EPA_DIST(nd) < epsilon)
	nd.vector = -nd.sm[3];

      DBG("[epa] pm: (%f, %f, %f) len: %f", nd.pm.x, nd.pm.y, nd.pm.z, norm(nd.pm));

      assert(msys_fabsf(dot(nd.sm[0] + v1 * k1, v1)) < epsilon);
      assert(msys_fabsf(dot(nd.sm[0] + v2 * k2, v2)) < epsilon);
      assert(msys_fabsf(dot(nd.pm, v1)) < epsilon);
      assert(msys_fabsf(dot(nd.pm, v2)) < epsilon);

      s.add(nd);
      return true;
    }

    bool epa_init(Array<gjk_data>& s, const gjk_data& ref, int i0, int i1, int i2)
    {
      gjk_data res;

      res.sm[3] = ref.sm[6 - i0 - i1 - i2]; // FIXME
      res.s1[0] = ref.s1[i0];
      res.s1[1] = ref.s1[i1];
      res.s1[2] = ref.s1[i2];
      res.s2[0] = ref.s2[i0];
      res.s2[1] = ref.s2[i1];
      res.s2[2] = ref.s2[i2];
      res.sm[0] = ref.sm[i0];
      res.sm[1] = ref.sm[i1];
      res.sm[2] = ref.sm[i2];

      return epa_insert(s, res);
    }

    bool epa_next(Array<gjk_data>& s, const gjk_data & nd, int index, const point3f & p1, const point3f & p2, const point3f & pm)
    {
      gjk_data nd2(nd);
      nd2.sm[3] = nd2.sm[index];
      nd2.s1[index] = p1;
      nd2.s2[index] = p2;
      nd2.sm[index] = pm;

      return epa_insert(s, nd2);
    }


    // EPA
    bool EPA(Mesh* m1, Mesh* m2, const gjk_data& data, point3f& out1, point3f& out2)
    {
      Array<gjk_data> s(128);

      DBG("[epa] init p0: (%f, %f, %f)", data.sm[0].x, data.sm[0].y, data.sm[0].z);
      DBG("[epa] init p1: (%f, %f, %f)", data.sm[1].x, data.sm[1].y, data.sm[1].z);
      DBG("[epa] init p2: (%f, %f, %f)", data.sm[2].x, data.sm[2].y, data.sm[2].z);
      DBG("[epa] init p3: (%f, %f, %f)", data.sm[3].x, data.sm[3].y, data.sm[3].z);

      bool init =
        epa_init(s, data, 0, 1, 2) ||
        epa_init(s, data, 0, 1, 3) ||
        epa_init(s, data, 0, 2, 3) ||
	epa_init(s, data, 1, 2, 3);
      assert(init);

      for (int i = 0; i < 32 && s.size > 0; ++i) // prevent infinite loops
      {
        int r = 0;
        for (int j = 1; j < s.size; ++j)
          if (EPA_DIST(s[j]) < EPA_DIST(s[r]))
            r = j;

        const gjk_data& ref = s[r];
        point3f p1 = support(m1,  ref.vector);
        point3f p2 = support(m2, -ref.vector);
        point3f pm = p1 - p2;
		float res = dot(pm - ref.pm, ref.vector);

        DBG("[epa] vector: (%f, %f, %f)", ref.vector.x, ref.vector.y, ref.vector.z);
        DBG("[epa] new point: (%f, %f, %f)", pm.x, pm.y, pm.z);
        DBG("[epa] res: %f", res);

        if (res < epsilon)
        {
          out1 = ref.p1;
          out2 = ref.p2;
          return true;
        }

        epa_next(s, ref, 0, p1, p2, pm);
        epa_next(s, ref, 1, p1, p2, pm);
        epa_next(s, ref, 2, p1, p2, pm);
        s.remove(r);
      }

      return false;
    }


  }


  // GJK

  bool GJK(Mesh* m1, Mesh* m2, point3f& out1, point3f& out2)
  {
    gjk_data data;
    vector3f init =
      m1->particles[0].position() -
      m2->particles[0].position();

    data.p1 = support(m1, -init);
    data.p2 = support(m2,  init);
    data.pm = data.p1 - data.p2;
    data1(data);

    for (int i = 0; i < 32; ++i) // prevent infinite loops
    {
      point3f base = data.pm;

      data.p1 = support(m1, -data.vector);
      data.p2 = support(m2,  data.vector);
      data.pm = data.p1 - data.p2;

      float res = dot(data.pm - base, data.vector);

      DBG("[gjk:%u] support: %f", i, dot(data.pm, -data.vector));
#ifdef DEBUG
      for (int j = 0; j < data.size; ++j)
        DBG("        %f, %f, %f", data.sm[j].x, data.sm[j].y, data.sm[j].z);
#endif
      DBG("[gjk:%u] vector: (%f, %f, %f) len: %f", i, data.vector.x, data.vector.y, data.vector.z, norm(data.vector));
      DBG("[gjk:%u] point:  (%f, %f, %f) len: %f", i, data.pm.x,     data.pm.y,     data.pm.z,     norm(data.pm    ));
      DBG("[gjk:%u] res:    %f", i, res);

      if (res >= -epsilon)
        return false;

      switch (data.size)
      {
        case 1:
          simp1(data);
          break;

        case 2:
          simp2(data);
          break;

        case 3:
          if (simp3(data))
          {
            out1.x = out1.y = out1.z = out2.x = out2.y = out2.z = 0.f;
            return EPA(m1, m2, data, out1, out2);
          }
          break;

        default:
          assert(false);
          break;
      }
    }

    return false; // should not happen
  }


}

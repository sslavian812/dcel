#include <vector>
#include <algorithm>

#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>
#include <cg/visualization/viewer_adapter.h>
#include <cg/primitives/point.h>

#include "line.h"
#include "orientation.h"


using cg::point_2f;
using std::pair;

struct sample_viewer : cg::visualization::viewer_adapter
{

    pair<int,int> getNearest(Line l)
    {
        for(int i=0; i<lines_.size()-1; ++i)
        {
            if(leftTurn(lines_[i].n, l.n ,lines_[i+1].n))
            {
                return std::make_pair(i, i+1);
            }
        }
        if(l<lines_[0])
            return std::make_pair(-1, 0);
        else if(lines_.back()<l)
            return std::make_pair(lines_.size()-1, -1);

        return std::make_pair(-2,-2); // PANIC!!! unreachable;
    }

   void draw(cg::visualization::drawer_type & drawer) const
   {
      drawer.set_color(Qt::white);
      for (int i = 0; i < lines_.size(); ++i)
      {
          Line l = lines_[i];
          drawer.draw_line(l.getSegment().first, l.getSegment().second);
      }

      if(current_line_)
      {
        Line cur = current_line_.get();
        drawer.set_color(Qt::green);
        drawer.draw_line(cur.getSegment().first, cur.getSegment().second);
      }

      if(first_)
      {
          Line l = first_.get();
          drawer.set_color(Qt::red);
          drawer.draw_line(l.getSegment().first, l.getSegment().second, 5);

      }
      if(second_)
      {
          Line r = second_.get();
          drawer.set_color(Qt::blue);
          drawer.draw_line(r.getSegment().first, r.getSegment().second, 5);
      }
   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "press lbutton to fix a point on line" << cg::visualization::endl
                        << "move mouse to specify line's normal vector" << cg::visualization::endl
                        << "lines count: " <<lines_.size() << cg::visualization::endl;
   }


   bool on_press(const point_2f & p)
   {
      if (current_point_)
         return false;

      current_point_ = p;
      return true;

      first_.reset();
      second_.reset();
   }

   bool on_release(const point_2f & p)
   {
      if (!current_point_)
         return false;
      if (!normal_point_)
         return false;

      Line l(current_point_.get(), normal_point_.get() - current_point_.get()); // Line(point_2f, vector_2f);

      first_.reset();
      second_.reset();

      if(lines_.size() >= 2)
      {
          pair<int, int> p = getNearest(l);
          if(p.first >= 0)
            first_ = lines_[p.first];
          if(p.second >= 0)
              second_= lines_[p.second];
      }

      lines_.push_back(l);
      sort(lines_.begin(), lines_.end());

      current_point_.reset();
      normal_point_.reset();


      return true;
   }

   bool on_move(const point_2f & p)
   {
       if (!current_point_)
          return false;

       if(!normal_point_ || normal_point_.get() != current_point_.get())
           normal_point_ = p;
       else
           return false;

       current_line_ = Line(current_point_.get(), normal_point_.get() - current_point_.get());
       return true;
    }

private:
   std::vector<Line> lines_;
   boost::optional<point_2f> current_point_;
   boost::optional<point_2f> normal_point_;
   boost::optional<Line> current_line_;
   boost::optional<Line> first_;
   boost::optional<Line> second_;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   sample_viewer viewer;
   cg::visualization::run_viewer(&viewer, "lines displayer");
}

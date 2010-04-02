module gubg.Grid;

//Creates a 2D grid on the rectangle (xs = [minX, maxX]) x (ys = [minY, maxY])
//dividing the xs interval in nrX equal subintervals (same for y)
//If needed, you can store objects of type CellT.
// * Set these objects into the cells using createCells(delegate)
class Grid(CellT = bool)
{
    this (uint nrX, uint nrY, real[2] xs, real[2] ys)
    {
        nrX_ = nrX;
        nrY_ = nrY;
        xInterval_[] = xs[];
        yInterval_[] = ys[];
    }

    //Iteration over all cells of the grid
    void each(void delegate(uint x, uint y) yield)
    {
        for (uint x = 0; x < nrX_; ++x)
            for (uint y = 0; y < nrY_; ++y)
                yield(x, y);
    }
    void each(void delegate(real cx, real cy) yield)
    {
        scope xcs = xCenters_;
        scope ycs = yCenters_;
        foreach (cx; xcs)
            foreach (cy; ycs)
                yield(cx, cy);
    }
    void each(void delegate(uint x, uint y, inout CellT cell) yield)
    {
        for (uint x = 0; x < nrX_; ++x)
            for (uint y = 0; y < nrY_; ++y)
                yield(x, y, cellsPerX_[x][y]);
    }
    void each(void delegate(real cx, real cy, inout CellT cell) yield)
    {
        scope xcs = xCenters_;
        scope ycs = yCenters_;
        foreach (x, cx; xcs)
            foreach (y, cy; ycs)
                yield(cx, cy, cellsPerX_[x][y]);
    }


    //Creation of the grid cells
    void createCells(CellT delegate(uint x, uint y) yield)
    {
        cellsPerX_.length = nrX_;
        foreach (x, inout cellsY; cellsPerX_)
        {
            cellsY.length = nrY_;
            foreach (y, inout cell; cellsY)
                cell = yield(x, y);
        }
     }
    void createCells(CellT delegate(real cx,real cy) yield)
    {
        scope xcs = xCenters_;
        scope ycs = yCenters_;
        cellsPerX_.length = nrX_;
        foreach (x, inout cellsY; cellsPerX_)
        {
            cellsY.length = nrY_;
            foreach (y, inout cell; cellsY)
                cell = yield(xcs[x], ycs[y]);
        }
     }

    real subWidth()
    {
        return (xInterval_[1] - xInterval_[0])/nrX_;
    }
    real subHeight()
    {
        return (yInterval_[1] - yInterval_[0])/nrY_;
    }

    private:
    real[] xCenters_()
    {
        real[] res = new real[nrX_];
        real xw = subWidth;
        foreach (i, inout x; res)
            x = xInterval_[0] + i*xw + 0.5*xw;
        return res;
     }
    real[] yCenters_()
    {
        real[] res = new real[nrY_];
        real yw = subHeight;
        foreach (i, inout y; res)
            y = yInterval_[0] + i*yw + 0.5*yw;
        return res;
     }

    uint nrX_;
    uint nrY_;
    real[2] xInterval_;
    real[2] yInterval_;
    CellT[][] cellsPerX_;
};

version (UnitTest)
{
    import gubg.Puts;
    void main()
    {
        //A simple grid without any cell type specified
        {
            scope grid = new Grid!()(3, 4, [0, 10], [0, 1]);
            void testUint(uint x, uint y)
            {
                puts("uint: x = {}, y = {}", x, y);
            }
            void testReal(real cx, real cy)
            {
                puts("real: cx = {}, cy = {}", cx, cy);
            }
            grid.each(&testUint);
            grid.each(&testReal);
        }

        //Creating cells
        {
            scope grid = new Grid!(real)(3, 4, [0, 10], [0, 1]);
            real createCellAsSum(real cx, real cy)
            {
                return cx + cy;
            }
            grid.createCells(&createCellAsSum);
            void test(real cx, real cy, inout real cell)
            {
                puts("real: cx = {}, cy = {}, cell = {}", cx, cy, cell);
            }
            grid.each(&test);
        }
    }
}

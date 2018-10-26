/*************************************************************************
sparse: Efficient Computation of the Flow Map from Sparse Samples

Author: Samer S. Barakat

Copyright (c) 2010-2013, Purdue University

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/
#include "DiscreteSibson.h"

//get function for the property map
My_point_property_map::reference 
get(My_point_property_map,My_point_property_map::key_type p)
{return boost::get<0>(p);}

#ifdef WIN32
double drand48()
{
    return ((double) rand() / (RAND_MAX));
}

int lrand48()
{
    return rand();
}

#endif



void dumpMallinfo(void) 
{
  struct mallinfo m = mallinfo();
  printf("uordblks = %zu, fordblks = %zu, hblkhd = %zu\n", m.uordblks, m.fordblks, m.hblkhd);
}

void VisFitPts(void* reconsc, vector<float3>& fitpts, vector<float3>& fitpts_n)
{
    printf("Start creating scene\n");
    NrrdWrapper3D* recons = (NrrdWrapper3D*) reconsc;
    vector<vtkActor*> actors;

    for (int i = 0; i < fitpts.size(); i+=10)
    {
        if (i%2000 == 0)
        {
            printf("."); fflush(stdout);
        }
        float3 pt = fitpts[i] / recons->min_spc;
        float3 nr = fitpts_n[i];

        // create sphere
        //vtkSphereSource* sphere = vtkSphereSource::New();
        //sphere->SetCenter(pt.x, pt.y, pt.z);
        //sphere->SetRadius(0.5);
        //sphere->Update();
        //vtkPolyDataMapper* sphereM = vtkPolyDataMapper::New();
        //sphereM->SetInputConnection(sphere->GetOutputPort());
        //sphereM->Update();
        //vtkActor* sphereA = vtkActor::New();
        //sphereA->SetMapper(sphereM);
        //sphereA->GetProperty()->SetColor(1.0, 0.0, 0.0);
        //actors.push_back(sphereA);
            
        // create arrows
        vtkArrowSource* arrowSource = vtkArrowSource::New();
        float3 startPoint = make_float3(pt.x, pt.y, pt.z);
        float3 endPoint = startPoint + 5 * nr;
        // The X axis is a vector from start to end
        float3 normalizedX = endPoint - startPoint;
        double lng = length(normalizedX);
        normalizedX = normalize(normalizedX);
        // The Z axis is an arbitrary vector cross X
        float3 arbitrary = make_float3(vtkMath::Random(-10,10), vtkMath::Random(-10,10), vtkMath::Random(-10,10));
        float3 normalizedZ = cross(normalizedX, arbitrary);
        normalizedZ = normalize(normalizedZ);
        // The Y axis is Z cross X
        float3 normalizedY = cross(normalizedZ, normalizedX);
        vtkMatrix4x4* matrix = vtkMatrix4x4::New();
        // Create the direction cosine matrix
        matrix->Identity();
        matrix->SetElement(0, 0, normalizedX.x);
        matrix->SetElement(0, 1, normalizedY.x);
        matrix->SetElement(0, 2, normalizedZ.x);
        matrix->SetElement(1, 0, normalizedX.y);
        matrix->SetElement(1, 1, normalizedY.y);
        matrix->SetElement(1, 2, normalizedZ.y);
        matrix->SetElement(2, 0, normalizedX.z);
        matrix->SetElement(2, 1, normalizedY.z);
        matrix->SetElement(2, 2, normalizedZ.z);
        // Apply the transforms
        vtkTransform* transform = vtkTransform::New();
        transform->Translate(startPoint.x, startPoint.y, startPoint.z);
        transform->Concatenate(matrix);
        transform->Scale(lng, lng, lng);
        //Create a mapper and actor for the arrow
        vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
        vtkActor* actor = vtkActor::New();
        #if USER_MATRIX
            mapper.SetInputConnection(arrowSource.GetOutputPort());
            actor.SetUserMatrix(transform.GetMatrix());
        #else
            // Transform the polydata
            vtkTransformPolyDataFilter* transformPD = vtkTransformPolyDataFilter::New();
            transformPD->SetTransform(transform);
            transformPD->SetInputConnection(arrowSource->GetOutputPort());
            mapper->SetInputConnection(transformPD->GetOutputPort());
        #endif
        mapper->Update();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0.0, 0.0, 1.0);
        actors.push_back(actor);

        // Create a circle
        vtkSmartPointer<vtkRegularPolygonSource> polygonSource =  vtkSmartPointer<vtkRegularPolygonSource>::New();
        polygonSource->SetNumberOfSides(10);
        polygonSource->SetRadius(3.0);
        polygonSource->SetCenter(pt.x, pt.y, pt.z);
        polygonSource->SetNormal(nr.x, nr.y, nr.z);
        mapper = vtkPolyDataMapper::New();
        actor = vtkActor::New();
        mapper->SetInputConnection(polygonSource->GetOutputPort());
        mapper->Update();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0.0, 0.0, 1.0);
        actors.push_back(actor);
    }

    // render window
    vtkRenderer *ren;
    vtkRenderWindow *renWin;
    ren = vtkRenderer::New();
    renWin = vtkRenderWindow::New();
    renWin->AddRenderer(ren);
    for (int i = 0; i < actors.size(); i++)
    {
        ren->AddActor(actors[i]);
    }
    ren->TwoSidedLightingOn();
    renWin->SetSize(768,768);
    renWin->Render();

    // export the scene
    vtkX3DExporter *exporter = vtkX3DExporter::New();
    exporter->SetInput(renWin);
    exporter->SetFileName("testX3DExporter.x3d");
    exporter->Update();
    exporter->Write();
    exporter->Print(std::cout);

    printf("Write X3D file\n");
}

////////////////////////////////////////////////////////////////////////////////
// find the potential at a point
////////////////////////////////////////////////////////////////////////////////
double FindPotential(NormalConstrainedSphericalMlsSurface* surface, Vector3f& cpt)
{
    // find the potential
    double b = surface->potentiel(cpt);
    if (abs(b) > 1e+6)
    {
        return b;
    }

    // project point
    Vector3f Cv = surface->mAlgebraicSphere.asSphere().mCenter;
    float3 C = make_float3(Cv.x, Cv.y, Cv.z);
    Vector3f ppt = cpt;
    Vector3f gpt;
    Expe::Color col;
    bool ret = surface->project(ppt, gpt, col);
    float3 A = make_float3(cpt.x, cpt.y, cpt.z);
    float3 B = make_float3(ppt.x, ppt.y, ppt.z);

    // check number of samples
    int nofn = surface->editNeighborhood()->getNofFoundNeighbors();
    if (nofn < 5)
    {
        return 1e9;
    }

    // find closest neighbor to B from two different sides
    double mind = numeric_limits<double>::max();
    float3 N1;
    double r1;
    for (int k = 0; k < nofn; k++)
    {
        float3 t = make_float3(surface->editNeighborhood()->getNeighbor(k).position().x, surface->editNeighborhood()->getNeighbor(k).position().y, surface->editNeighborhood()->getNeighbor(k).position().z);
        if (length(t - B) < mind)
        {
            mind = length(t - B);
            N1 = t;
            r1 = surface->editNeighborhood()->getNeighbor(k).radius();
        }
    }
    if (dot(normalize(C - B), normalize(C - N1)) < 0.9)
    {
        return 1e9;
    }

    // if distance is far smaller than nearest neighbor
    if ((abs(b) < 0.5 * length(A-N1)) && (length(A-N1) > 0.5 * r1))
    {
        return 1e9;
    }

    return b;
}

////////////////////////////////////////////////////////////////////////////////
// find the closest to each point
////////////////////////////////////////////////////////////////////////////////

void FindClosest(
        void* reconsc,
        vector<closest_site>& query_cls,
        vector<NaturalNeighbors>& query_nc,
        vector<Sample_point>& pts, 
        vector<bool>& site_is_disc,
        Tree*& tree,
        int nosurf,
        vector<NormalConstrainedSphericalMlsSurface*>& surfaces,
        vector<set<int> >& site2discs)
{
    NrrdWrapper3D* recons = (NrrdWrapper3D*) reconsc;
    double min_spc = recons->min_spc;

    // Build the kd-tree
    if (tree == NULL)
    {
        std::vector<Point_3> tree_points;
        std::vector<int> tree_indices;
        for (int i = 0; i < pts.size(); i++)
        {
            // exclude sites of the discontinuity
            if (site_is_disc[i] == true)
                continue;
            tree_points.push_back(Point_3(pts[i].coordinate.x,pts[i].coordinate.y,pts[i].coordinate.z));
            tree_indices.push_back(i);
        }
        tree = new Tree(
            boost::make_zip_iterator(boost::make_tuple( tree_points.begin(),tree_indices.begin() )),
            boost::make_zip_iterator(boost::make_tuple( tree_points.end(),tree_indices.end() ))  
        );
    }

    // Find the closest site to each of the grid points
    query_cls.resize(recons->Size());
    #pragma omp parallel for
    for (int i = 0; i < query_cls.size(); i++)
    {
        float3 qc = recons->Addr2Space(i);

        Point_3 query(qc.x, qc.y, qc.z);
        K_neighbor_search search(*tree, query, 1);
        Distance tr_dist;
        query_cls[i].id = boost::get<1>(search.begin()->first);
        query_cls[i].dist = tr_dist.inverse_of_transformed_distance(search.begin()->second);
        if (nosurf == 0)
            continue;

        // check if it has a natural neighbor for a particular surface
        // requires that the natural coordinates be available
        vector<bool> ps(nosurf);
        for (int it = 0; it < query_nc[i].size(); it++)
        {
            int site = query_nc[i].nv[it];
            for (set<int>::iterator it2 = site2discs[site].begin(); it2 != site2discs[site].end(); it2++)
            {
                ps[*it2] = true;
            }
        }

        // now check closest surface that is 
        int thn = omp_get_thread_num();
        int soff = thn * nosurf;
        Vector3f cpt = Vector3(qc.x / min_spc, qc.y / min_spc, qc.z / min_spc);
        for (int k = 0; k < nosurf; k++)
        {
            if (!ps[k])
                continue;
            double p = FindPotential(surfaces[soff + k], cpt);
            if ((abs(p) * min_spc) < query_cls[i].dist)
            {
                query_cls[i].id = -(k + 1);
                query_cls[i].dist = abs(p) * min_spc;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// find the natural coordinates for each point
////////////////////////////////////////////////////////////////////////////////

void FindNaturalCoordinates(
        void* reconsc,
        vector<closest_site>& query_cls,
        vector<NaturalNeighbors>& query_nc,
        vector<Sample_point>& pts, 
        int nosurf,
        vector<NormalConstrainedSphericalMlsSurface*>& surfaces)
{
    NrrdWrapper3D* recons = (NrrdWrapper3D*) reconsc;
    double min_spc = recons->min_spc;

    Timer timer;
    timer.start();

    // clear old data
    #pragma omp parallel for
    for (int i = 0; i < query_cls.size(); i++)
    {
        query_nc[i].reset();
    }

    // Compute the natural neighbors for each grid point
    vector<omp_lock_t> writelock(query_nc.size());
    for (int i = 0; i < writelock.size(); i++)
    {
        omp_init_lock(&(writelock[i]));
    }
    int l1 = recons->width() - 1;
    int l2 = recons->height() - 1;
    int l3 = recons->depth() - 1;
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < query_cls.size(); i++)
    {
        int3 c = recons->Addr2Coord(i);

        // find its closest site info
        int id = query_cls[i].id;
        float fdist = query_cls[i].dist;

        // compute the distance in grid space
        fdist = fdist / min_spc;

        // assumes the data grid is isotropic
        int dist = ceil(fdist);

        // squared distance
        fdist *= fdist;
        
        // now loop on all points in range
        int3 d;
        vector<int> idxs;
        for (int x = c.x - dist; x <= c.x + dist; x++)
        {
            if ((x < 0) || (x > l1))
                continue;
            d.x = x - c.x;
            d.x *= d.x;

            for (int y = c.y - dist; y <= c.y + dist; y++)
            {
                if ((y < 0) || (y > l2))
                    continue;
                d.y = y - c.y;
                d.y *= d.y;
                if ((d.x + d.y) > fdist)
                    continue;

                for (int z = c.z - dist; z <= c.z + dist; z++)
                {
                    if ((z < 0) || (z > l3))
                        continue;
                    d.z = z - c.z;
                    d.z *= d.z;        
                    if ((d.x + d.y + d.z) > fdist)
                        continue;

                    int idx = recons->Coord2Addr(x, y, z);
                    idxs.push_back(idx);
                }
            }
        }

        // loop idxs
        for (int k = 0; k < idxs.size(); k++)
        {
            int idx = idxs[k];
            int it = query_nc[idx].find(id);
            if (it < 0)
            {
                // lock only the map at idx for insertion
                omp_set_lock(&(writelock[idx]));
                if ((it = query_nc[idx].find(id)) < 0)
                {
                    it = query_nc[idx].insert(id, 0.0);
                }
                omp_unset_lock(&(writelock[idx]));
            }
            
            // update the value
            float* fp = &(query_nc[idx].nw[it]);
            #pragma omp atomic
            (*fp) += 1.0;
        }

        if ((i%1048576) == 0)
        {
            printf("."); fflush(stdout);
        }
    }
    for (int i = 0; i < query_nc.size(); i++)
    {
        omp_destroy_lock(&(writelock[i]));
    }

    timer.stop();
    cout << "\nTime for natural neighbors computation is " << (0.001 * timer.getElapsedTimeInMilliSec()) << " sec.\n";

    // find the maximum number of natural neighbors
    //int maxi = 0;
    //for (int i = 0; i < query_nc.size(); i++)
    //{
    //    maxi = max(maxi, query_nc[i].size());
    //}
    //printf("Maximum number of natural neighbors is %d\n", maxi);

    // a natural neighbor that has a zero weight
    #pragma omp parallel for
    for (int i = 0; i < query_nc.size(); i++)
    {
        query_nc[i].removezero();
    }

    // when distance is zero only one neighbor should remain
    #pragma omp parallel for
    for (int i = 0; i < query_cls.size(); i++)
    {
        if (query_cls[i].dist == 0.0)
        {
            query_nc[i].clear();
            query_nc[i].insert(query_cls[i].id, 1.0);
        }
    }

    // Normalize natural coordinates
    #pragma omp parallel for
    for (int i = 0; i < query_cls.size(); i++)
    {
        float sum = 0.0;
        for (int itc = 0; itc < query_nc[i].size(); itc++)
        {
            sum += query_nc[i].nw[itc];
        }
        for (int itc = 0; itc < query_nc[i].size(); itc++)
        {
            query_nc[i].nw[itc] /= sum;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// find the surface fit value
////////////////////////////////////////////////////////////////////////////////

inline double wendland(double r, double radius)
{
    double __r = r / radius;
    if (__r >= 1.) return 0;
    return pow(1. - __r, 4.)*(4.*__r + 1);
}

bool Cmp1Dfit(float4 i, float4 j) { return (i.z<j.z); }

int FindSurfaceFit(
        NrrdWrapper3D* recons,
        vector<closest_site>& query_cls,
        vector<NaturalNeighbors>& query_nc,
        vector<Sample_point>& pts, 
        int nosurf,
        vector<NormalConstrainedSphericalMlsSurface*>& surfaces,
        int qid,
        int surf_no,
        double& ptdist,
        double& retval,
        vector<vector<float> >& sites_pot,
        vector<vector<float> >& sites_pgr)    
{
    double min_spc = recons->min_spc;
    int thn = omp_get_thread_num();
    int soff = thn * nosurf;
    float3 P = recons->Addr2Space(qid);

    // rets: 0 failed, 1 succeeded, 2 out of range but succeeded
    int rets = 1;

    // distance from the point to the surface
    Vector3f cpt = Vector3(P.x / min_spc, P.y / min_spc, P.z / min_spc);
    double qx = FindPotential(surfaces[soff + surf_no], cpt);
    ptdist = abs(qx);
    if (ptdist > 1e+6)
    {
        // error
        return 0;
    }

    // list of all data and point ids
    vector<float4> xy;
    set<int> nids;

    // get points that are surface neighbors
    int nofn = surfaces[soff + surf_no]->editNeighborhood()->getNofFoundNeighbors();
    for (int k = 0; k < nofn; k++)
    {
        PointSet::ConstPointHandle cph = surfaces[soff + surf_no]->editNeighborhood()->getNeighbor(k);
        nids.insert(cph.siteid());
    }

    // add the natural neighbors too
    for (int i = 0; i < query_nc[qid].size(); i++)
    {
        if (query_nc[qid].nv[i] >= 0)
            nids.insert(query_nc[qid].nv[i]);
    }
    
    // get the info for each id
    for (set<int>::iterator it = nids.begin(); it != nids.end(); it++)
    {
        // position and weight
        float4 cxy;
        cxy.x = sites_pot[*it][surf_no];
        if ((cxy.x > 1e6) || (cxy.x == 0.0))
        {
            continue;
        }
        cxy.y = pts[*it].value;
        cxy.z = length(pts[*it].coordinate - P) + 1e-6;
        cxy.z /= recons->min_spc;
        //cxy.w = sites_pgr[*it][surf_no];
        xy.push_back(cxy);
    }

    // other parameters
    double lx = numeric_limits<double>::max();
    double hx = -numeric_limits<double>::max();
    double ly = numeric_limits<double>::max();
    double hy = -numeric_limits<double>::max();
    for (int k = 0; k < xy.size(); k++)
    {
        lx = min(double(lx), double(xy[k].x));
        hx = max(double(hx), double(xy[k].x));
        ly = min(double(ly), double(xy[k].y));
        hy = max(double(hy), double(xy[k].y));
    }
    if (xy.size() < 3)
    {
        // error
        return 0;
    }
    if (qx < lx)
    {
        qx = lx;
        rets = 2;
    }
    if (qx > hx)
    {
        qx = hx;
        rets = 2;
    }

    // sort based on distance closest first and delete items beyond 20
    sort(xy.begin(), xy.end(), Cmp1Dfit);
    if (xy.size() > 32)
    {
        xy.erase(xy.begin() + 32, xy.end());
    }
    




    


    
    /*

    // do MLS
    double radius = 0.0;
    for (int i = 0; i < xy.size(); i++)
    {
        radius = max(radius, xy[i].z);
    }
    radius /= 3.0;
    int nrows = xy.size() * 2; // value and three gradients
    int ncols = 4; // 13 coefficients
    if (nrows < ncols)
    {
        return 0;
    }
    double C[10];
    double chisq;
    gsl_matrix *X, *cov;
    gsl_vector *y, *w, *c;
    X = gsl_matrix_alloc (nrows, ncols);
    y = gsl_vector_alloc (nrows);
    w = gsl_vector_alloc (nrows);
    c = gsl_vector_alloc (ncols);
    cov = gsl_matrix_alloc (ncols, ncols);
    int k = 0;
    for (int i = 0; i < xy.size(); i++)
    {
        // F
        for (int q = 0; q < ncols; q++)
        {
            gsl_matrix_set (X, k, q, pow(xy[i].x, q));
        }
        gsl_vector_set(y, k, xy[i].y);
        gsl_vector_set(w, k, 1.0 / xy[i].z);
        k++;

        // dF/dx
        for (int q = 0; q < ncols; q++)
        {
            gsl_matrix_set (X, k, q, q * pow(xy[i].x, q - 1));
        }
        gsl_vector_set(y, k, xy[i].w);
        gsl_vector_set(w, k, 0.2 / xy[i].z);
        k++;
    }
    gsl_multifit_linear_workspace* work = gsl_multifit_linear_alloc (nrows, ncols);
    gsl_multifit_wlinear(X, w, y, c, cov, &chisq, work);
    for (int i = 0; i < ncols; i++)
    {
        C[i] = gsl_vector_get(c, i);
    }
    gsl_multifit_linear_free (work);
    gsl_matrix_free (X);
    gsl_vector_free (y);
    gsl_vector_free (w);
    gsl_vector_free (c);
    gsl_matrix_free (cov);
    double ret = 0.0;
    for (int q = 0; q < ncols; q++)
    {
        ret += C[q] * pow(qx, q);
    }


    retval = ret;
    return rets;
    
    */

    
    


    

    
    // do the fitting
    int error = 0;
    double qy;
    try
    {
        qy = CircleFitting(xy, lx, hx, ly, hy, qx, error);
    }
    catch(int e)
    {
        return 0;
    }
    if (error != 0)
        return 0;
    retval = qy;
    return rets;
}

////////////////////////////////////////////////////////////////////////////////
// find Sibson value
////////////////////////////////////////////////////////////////////////////////


float4 CircleCoords(float3 p_i, float3 f_c, float f_r)
{
    // project the point to surface
    float3 p = p_i - f_c;
    p = (f_r / length(p)) * p;
    float3 r_i = f_c + p;

    // now find u,v http://wiki.answers.com/Q/What_is_the_parameterization_of_a_sphere
    //x = x0 + r sin u cos v
    //y = y0 + r sin u sin v
    //z = z0 + r cos u
    //0 <= u <= pi
    //0 <= v <= 2 pi

    double u_i = acos((r_i.z - f_c.z) / f_r);
    double v_i = acos((r_i.x - f_c.x) / (f_r * sin(u_i)));
    if (abs(r_i.y - f_c.y - f_r * sin(u_i) * sin(v_i)) > abs(r_i.y - f_c.y - f_r * sin(u_i) * sin(-v_i)))
    {
        v_i = -v_i;
    }
    double w_i = length(p_i - r_i);
    if (length(p_i - f_c) > f_r)
        w_i = -w_i;

    //printf("%f %f %f -> %f %f %f\n", r_i.x, r_i.y, r_i.z, f_c.x + f_r * sin(u_i) * cos(v_i), f_c.y + f_r * sin(u_i) * sin(v_i), f_c.z + f_r * cos(u_i));

    return make_float4(u_i, v_i, w_i, 0.0);
}

double SibsonInterpolation(
        NrrdWrapper3D* recons,
        vector<float>& errm,
        vector<closest_site>& query_cls,
        vector<NaturalNeighbors>& query_nc,
        vector<Sample_point>& pts, 
        Tree*& tree,
        int nosurf,
        vector<NormalConstrainedSphericalMlsSurface*>& surfaces,
        int qid,
        vector<vector<float> >& sites_pot,
        vector<vector<float> >& sites_pgr)
{
    float3 P = recons->Addr2Space(qid);
    vector<float2> wv;

    // Sibson's interpolation
    float3 p_i;
    double z_i;
    float3 g_i;
    double l_i;
    double d_i;
    double xi_i = 0.0;
    double xi = 0.0;
    double xi_num = 0.0;
    double xi_den = 0.0;
    double Z0 = 0.0;
    double alpha = 0.0;
    double beta = 0.0;
    double alpha_num = 0.0;
    double alpha_den = 0.0;
    int i = 0;
    for (int it = 0; it < query_nc[qid].size(); it++)
    {
        double ptdist;
        double retval;
        int id = query_nc[qid].nv[it];
        if (id >= 0)
        {
            p_i = pts[id].coordinate;
            l_i = query_nc[qid].nw[it];
            z_i = pts[id].value;
            g_i = make_float3(pts[id].gradient[0], pts[id].gradient[1], pts[id].gradient[2]);
        }
        else
        {
            id = -id - 1;
            int status = 0;
            status = FindSurfaceFit(recons, query_cls, query_nc, pts, nosurf, surfaces, qid, id, ptdist, retval, sites_pot, sites_pgr);
            if (status == 0)
            {
                // if error occured use value from regular sibson
                int3 c = recons->Addr2Coord(qid);
                return recons->ProbeValueAt(c.x, c.y, c.z);
            }
            ptdist *= recons->min_spc;

            p_i = make_float3(0.0);
            l_i = query_nc[qid].nw[it];
            z_i = retval;
            g_i = make_float3(0.0);
        }
        i++;

        // update the values
        float3 d = P - p_i;
        double sl = dot(d,d);
        double l = sqrt(sl);
        if (query_nc[qid].nv[it] < 0)
        {
            sl = ptdist*ptdist;
            l = ptdist;
        }
        double f = l;

        // update the rest
        if (l_i == 0.0)
            continue;
        if (f == 0.0)
        {
            errm[qid] = 0.0;
            return z_i;
        }

        // compute Z0
        Z0 += l_i * z_i;

        // now compute xi_i
        xi_i = z_i + dot(g_i, d); 

        // xi computation
        xi_num += l_i * xi_i / f;
        xi_den += l_i / f;

        // alpha now
        alpha_num += l_i * sl / f;
        alpha_den += l_i / f;

        // beta next
        beta += l_i * sl;

        // computing the weighted variance
        wv.push_back(make_float2(l_i / f, xi_i));
    }

    // final result
    alpha = alpha_num / alpha_den;
    xi = xi_num / xi_den;
    double res;
    res = (alpha * Z0 + beta * xi) / (alpha + beta);

    // compute the weighted variance
    double wvv = 0.0;
    for (int i = 0; i < wv.size(); i++)
    {
        wvv = wv[i].x * pow(xi - wv[i].y, 2.0); // (l_i / f) * (xi - xi_i)^2
    }
    wvv /= xi_den;
    errm[qid] = wvv;

    return res;
}

////////////////////////////////////////////////////////////////////////////////
// adjust the normals directions
////////////////////////////////////////////////////////////////////////////////

void AdjustPointNormals(vector<float3>& fitpts, vector<float3>& fitpts_n)
{
    // Build the kd-tree
    std::vector<Point_3> points;
    std::vector<int> indices;
    Tree tree;
    vector<bool> done(fitpts.size());
    vector<closest_site> clss(fitpts.size());

    // add vertex 0
    done[0] = true;
    points.push_back(Point_3(fitpts[0].x, fitpts[0].y, fitpts[0].z));
    indices.push_back(0);
    tree.insert(
        boost::make_zip_iterator(boost::make_tuple( points.begin(),indices.begin() )),
        boost::make_zip_iterator(boost::make_tuple( points.end(),indices.end() ) )  
    );

    // loop until tree has all
    while (true)
    {
        #pragma omp parallel for
        for (int i = 0; i < fitpts.size(); i++)
        {
            if (done[i])
                continue;
            Point_3 query(fitpts[i].x, fitpts[i].y, fitpts[i].z);
            K_neighbor_search search(tree, query, 1);
            Distance tr_dist;
            clss[i].id = boost::get<1>(search.begin()->first);
            clss[i].dist = tr_dist.inverse_of_transformed_distance(search.begin()->second);
        }

        double mind = numeric_limits<double>::max();
        for (int i = 0; i < fitpts.size(); i++)
        {
            if (done[i])
                continue;
            mind = min(mind, double(clss[i].dist));
        }
        if (mind == numeric_limits<double>::max())
            break;

        points.clear();
        indices.clear();
        //#pragma omp parallel for
        for (int i = 0; i < fitpts.size(); i++)
        {
            if (done[i])
                continue;
            if (clss[i].dist != mind)
                continue;
            if (dot(fitpts_n[i], fitpts_n[clss[i].id]) < 0.0)
                fitpts_n[i] = -fitpts_n[i];
            done[i] = true;
            #pragma omp critical
            {
                points.push_back(Point_3(fitpts[i].x, fitpts[i].y, fitpts[i].z));
                indices.push_back(i);
            }
        }
        tree.insert(
            boost::make_zip_iterator(boost::make_tuple( points.begin(),indices.begin() )),
            boost::make_zip_iterator(boost::make_tuple( points.end(),indices.end() ) )  
        );
    }
}

////////////////////////////////////////////////////////////////////////////////
// find the discontinuities
////////////////////////////////////////////////////////////////////////////////

void FindDiscontinuitySignal(int iter, int field, NrrdWrapper3D* recons, vector<Sample_point>& pts, string infile, string outfile)
{
    // Find the min and max values from samples
    double minv = numeric_limits<double>::max();
    double maxv = -numeric_limits<double>::max();
    for (int i = 0; i < pts.size(); i++)
    {
        minv = min(minv, pts[i].value);
        maxv = max(maxv, pts[i].value);
    }

    // find the damped min and max gradient magnitude
    double ming = numeric_limits<double>::max();
    double maxg = -numeric_limits<double>::max();
    for (int i = 0; i < recons->Size(); i++)
    {
        int3 c = recons->Addr2Coord(i);
        float3 g = recons->ProbeGradAt(c.x, c.y, c.z);
        double gm = length(g);
        if (gm == 0.0)
            continue;

        gm = -log(gm);
        ming = min(ming, gm);
        maxg = max(maxg, gm);
    }
    
    // put all the points in bins 
    int nobins = 256;
    vector<vector<int> > bins(nobins);
    for (int i = 0; i < recons->Size(); i++)
    {
        int3 c = recons->Addr2Coord(i);
        float3 g = recons->ProbeGradAt(c.x, c.y, c.z);
        double gm = length(g);
        if (gm == 0.0)
            continue;

        gm = -log(gm);
        int idx = myround(nobins * (gm - ming) / (maxg - ming));
        idx = min(idx, nobins - 1);
        bins[idx].push_back(i);
    }
    // print the size of bins
    //for (int i = 0; i < bins.size(); i++)
    //{
    //    printf("%d\n", bins[i].size());
    //}

    // sampling ratio
    double ratio = double(pts.size()) / recons->Size();

    // read user input
    double p1;
    double p2;
    if (field == 0)
    {
        p1 = atof(parameters["UPPER_THRES"].c_str());
        p2 = atof(parameters["LOWER_THRES_0"].c_str());
    }
    else if (field == 1)
    {
        p1 = atof(parameters["UPPER_THRES"].c_str());
        p2 = atof(parameters["LOWER_THRES_1"].c_str());
    }
    else
    {
        p1 = atof(parameters["UPPER_THRES"].c_str());
        p2 = atof(parameters["LOWER_THRES_2"].c_str());
    }

    // scale
    p1 *= recons->Size();
    p2 *= recons->Size();

    // compute the upper threshold
    int k = 0;
    int sum = 0;
    for (k = 0; k < bins.size(); k++)
    {
        sum += bins[k].size();
        if (sum > p1)
            break;
    }
    double v1 = (double(k+1) * (maxg - ming)) / nobins + ming;
    v1 = exp(-v1);

    // compute the upper threshold
    sum = 0;
    for (k = 0; k < bins.size(); k++)
    {
        sum += bins[k].size();
        if (sum > p2)
            break;
    }
    double v2 = (double(k+1) * (maxg - ming)) / nobins + ming;
    v2 = exp(-v2);

    // scale v1 if needed
    v1 = min(v1, 2.0 * v2);

    // scale the threshold value
    if (field == 2)
    {
        v1 = 95.0;
        v2 = 85.0;
    }
    double upperThreshold = v1 * recons->min_spc;
    double lowerThreshold = v2 * recons->min_spc;
    printf("Actual upper thres. is %lf and lower thres. is %lf\n", v1, v2);

    // do the canny edge detection
    double variance = 3.0 * recons->min_spc * recons->min_spc;
    double maxError = 0.005;
    itk::NrrdImageIO::Pointer io = itk::NrrdImageIO::New();
    io->SetFileType( itk::ImageIOBase::Binary );
    RealReaderType::Pointer reader = RealReaderType::New();
    reader->SetFileName( infile );
    reader->SetImageIO(io);
    reader->Update();
    CannyFilter::Pointer cannyFilter = CannyFilter::New();
    cannyFilter->SetVariance( variance );
    cannyFilter->SetMaximumError( maxError );
    cannyFilter->SetUpperThreshold( upperThreshold );
    cannyFilter->SetLowerThreshold( lowerThreshold );
    cannyFilter->SetInput( reader->GetOutput() );
    cannyFilter->Update();
    RescaleFilter::Pointer rescale = RescaleFilter::New();
    rescale->SetOutputMinimum(0);
    rescale->SetOutputMaximum(255);
    rescale->SetInput( cannyFilter->GetOutput() );
    rescale->Update();
    CharWriterType::Pointer writer = CharWriterType::New();
    writer->UseInputMetaDataDictionaryOn();
    writer->SetInput( rescale->GetOutput() );
    writer->SetImageIO(io);
    writer->SetFileName(outfile);
    writer->Update();
}

////////////////////////////////////////////////////////////////////////////////
// find connected compoenets
////////////////////////////////////////////////////////////////////////////////

void FindConnectedComponents(NrrdWrapper3D* recons, string infile, string outfile, vector<vector<int> >& comps)
{
    Nrrd* nin = readNrrd(infile.c_str());
    Nrrd *nou = nrrdNew();
    nrrdCCFind(nou, NULL, nin, nrrdTypeDefault, 3);
    
    if (nrrdSave(outfile.c_str(), nou, NULL)) {
        cerr << "Error saving: " << biffGetDone(NRRD) << std::endl;
        exit(-1);
    }

    // create the sets
    for (int x = 0; x < recons->width(); x++)
    {
        for (int y = 0; y < recons->height(); y++)
        {
            for (int z = 0; z < recons->depth(); z++)
            {
                int id = recons->Coord2Addr(x,y,z);
                int cp = ((char*)nou->data)[id];
                if (cp == 0)
                    continue;
                int idx = recons->Coord2Addr(x, y, z);
                if (comps.size() < cp)
                    comps.resize(cp);
                comps[cp - 1].push_back(int(idx));
            }
        }
    }

    nrrdNuke(nin);
    nrrdNuke(nou);
}

////////////////////////////////////////////////////////////////////////////////
// find the sites of the discontinuities and reduce the points to fit
////////////////////////////////////////////////////////////////////////////////

void FindDiscSites(
    NrrdWrapper3D* recons,
    vector<closest_site>& query_cls,
    vector<NaturalNeighbors>& query_nc,
    vector<Sample_point>& pts, 
    vector<vector<int> >& comps,
    vector<set<int> >& site2discs)
{
    for (int i = 0; i < comps.size(); i++)
    {
        map<int, int> site_point;
        map<int, double> site_dist;

        // find the sites of each discontinuity
        for (int k = 0; k < comps[i].size(); k++)
        {
            int pt = comps[i][k];

            // loop on natural neighbors of the point
            for (int itc = 0; itc < query_nc[pt].size(); itc++)
            {
                int site = query_nc[pt].nv[itc];
                site2discs[site].insert(i);

                // check the distance between point to site
                double d = length(recons->Addr2Space(pt) - pts[site].coordinate);
                if ((site_dist.find(site) == site_dist.end()) || (d < site_dist[site]))
                {
                    site_point[site] = pt;
                    site_dist[site] = d;
                }
            }
        }

        // now find the points to use for the fitting
        set<int> nl;
        for (map<int, int>::iterator it = site_point.begin(); it != site_point.end(); it++)
        {
            // points near the boundary don't have gradients!
            int3 c = recons->Addr2Coord(it->second);
            float l = length(recons->ProbeGradAt(c.x, c.y, c.z));
            if (l != 0.0)
            {
                nl.insert(int(it->second));
            }
        }
        comps[i].clear();
        comps[i].insert(comps[i].begin(), nl.begin(), nl.end());

        // print the content
        //for (int k = 0; k < comps[i].size(); k++)
        //    printf("%d ", comps[i][k]);
        //printf("\n");
    }
}

////////////////////////////////////////////////////////////////////////////////
// find the discontinuity surfaces through fitting
////////////////////////////////////////////////////////////////////////////////

void FindDiscSurfaces(
    NrrdWrapper3D* recons,
    vector<closest_site>& query_cls,
    vector<NaturalNeighbors>& query_nc,
    vector<Sample_point>& pts, 
    int nosurf,
    vector<NormalConstrainedSphericalMlsSurface*>& surfaces,
    vector<vector<int> >& comps)
{
    // create surfaces with points
    double min_spc = recons->min_spc;
    int omptn = omp_get_max_threads();
    int numsurfs = comps.size();
    surfaces.resize(omptn * numsurfs);
    for (int i = 0; i < comps.size(); i++)
    {
        printf("%d (%d),", i, comps[i].size());

        // find info for fitting
        vector<float3> fitpts(comps[i].size());
        vector<float3> fitpts_n(comps[i].size());
        vector<int> fitpts_e(comps[i].size());
        for (int k = 0; k < comps[i].size(); k++)
        {
            int id = comps[i][k];
            fitpts[k] = recons->Addr2Space(id);
            int3 c = recons->Addr2Coord(id);
            fitpts_n[k] = normalize(recons->ProbeGradAt(c.x, c.y, c.z));
            fitpts_e[k] = query_cls[id].id;
        }

        //VisFitPts(recons, fitpts, fitpts_n);
        // Adjust the normals (may be not needed anymore)
        //AdjustPointNormals(fitpts, fitpts_n);

        // create and fill the point set with normals
        double average_radius = 0.0;
        PointSet* pPointsNormals = new PointSet(PointSet::Attribute_position
                    | PointSet::Attribute_normal
                    | PointSet::Attribute_radius
                    | PointSet::Attribute_siteid);
        for (int k = 0; k < fitpts.size(); k++)
        {
            PointSet::PointHandle pt = pPointsNormals->append();
            pt.position() = Vector3(fitpts[k].x / min_spc, fitpts[k].y / min_spc, fitpts[k].z / min_spc);
            pt.normal() = Vector3(fitpts_n[k].x, fitpts_n[k].y, fitpts_n[k].z);

            // find radius through natural neighbors
            int id = comps[i][k];
            float3 p1 = recons->Addr2Space(id);
            double radius = numeric_limits<double>::min();
            for(int itc = 0; itc < query_nc[id].size(); itc++)
            {
                int it = query_nc[id].nv[itc];
                float3 p2 = pts[it].coordinate;
                radius = max(radius, double(length(p1 - p2)));
            }
            radius *= 2.0 / min_spc;
            pt.radius() = radius;
            pt.siteid() = fitpts_e[k];
            average_radius += radius;
        }
        average_radius /= pPointsNormals->size();

        // some points have only single natural neighbor use the average then
        for (int k = 0; k < fitpts.size(); k++)
        {
            int id = comps[i][k];
            if (query_nc[id].size() != 1)
                continue;
            pPointsNormals->at(k).radius() = average_radius;
        }

        // fitting
        #pragma omp parallel for
        for (int j = 0; j < omptn; j++)
        {
            NormalConstrainedSphericalMlsSurface* ncsurface = new NormalConstrainedSphericalMlsSurface(pPointsNormals);
            BallNeighborhood* ballneighborhood = new BallNeighborhood(ncsurface->mpInputPoints);
            Wf_OneMinusX2Power4* weightfunction = new Wf_OneMinusX2Power4();
            ncsurface->setNeighborhood(ballneighborhood);
            ncsurface->setWeightingFunction(weightfunction);
            surfaces[j * numsurfs + i] = ncsurface;

            Vector3f cpt = Vector3(0.0,0.0,0.0);
            surfaces[j * numsurfs + i]->potentiel(cpt);
        }
    }
    printf("\n");
}

////////////////////////////////////////////////////////////////////////////////
// adjust the normals directions
////////////////////////////////////////////////////////////////////////////////

void DiscreteSisbon(
    void* originc, 
    void* reconsc, 
    vector<float>& errm, 
    vector<Sample_point>& pts, 
    Tree*& tree, 
    vector<closest_site>& query_cls,
    vector<NaturalNeighbors>& query_nc)
{
    // main variables
    NrrdWrapper3D* origin = (NrrdWrapper3D*) originc;
    NrrdWrapper3D* recons = (NrrdWrapper3D*) reconsc;
    vector<NormalConstrainedSphericalMlsSurface*> surfaces;
    int nosurf = 0;

    // sibson interpolation
    vector<vector<float> > sites_pot;
    vector<vector<float> > sites_pgr;
    #pragma omp parallel for
    for (int i = 0; i < query_cls.size(); i++)
    {
        double msibv = SibsonInterpolation(recons, errm, query_cls, query_nc, pts, tree, nosurf, surfaces, i, sites_pot, sites_pgr);
        int3 c = recons->Addr2Coord(i);
        recons->Set(c.x, c.y, c.z, msibv);
    }

    // compute mse 
    double mse = 0.0;
    for (int i = 0; i < query_cls.size(); i++)
    {
        int3 c = recons->Addr2Coord(i);
        double v = origin->ProbeValueAt(c.x, c.y, c.z);
        mse += pow(v - recons->ProbeValueAt(c.x, c.y, c.z), 2.0);
    }
    mse /= query_cls.size();
    printf("MSE error is %e\n", mse);
}

void DiscreteSisbonWithSurfaces(
    int iter,
    int field,
    void* originc, 
    void* reconsc, 
    vector<float> errm, 
    vector<Sample_point>& pts, 
    vector<closest_site> query_cls,
    vector<NaturalNeighbors> query_nc)
{
    // main variables
    NrrdWrapper3D* origin = (NrrdWrapper3D*) originc;
    NrrdWrapper3D* recons = (NrrdWrapper3D*) reconsc;
    vector<set<int> > site2discs(pts.size());
    vector<NormalConstrainedSphericalMlsSurface*> surfaces;
    int nosurf = 0;

    // edge file name
    char iters[12];
    sprintf(iters, "%d", iter);
    char fields[12];
    sprintf(fields, "%d", field);
    string filename = parameters["OUTPUT_EDGES"] + string("_") + string(fields) + string("_") + string(iters) + string(".nrrd");

    // start timing
    Timer timer;
    timer.start();
    
    // save the file
    recons->Write("sibtmp.nrrd");

    // find the edges from the signal
    FindDiscontinuitySignal(iter, field, recons, pts, string("sibtmp.nrrd"), filename);

    // read the edges image and find connected components
    vector<vector<int> > comps;
    FindConnectedComponents(recons, filename, filename, comps);
    nosurf = comps.size();
    printf("Number of surfaces is %d.\n", nosurf); 

    // find the sample sites of each discontinuity
    FindDiscSites(recons, query_cls, query_nc, pts, comps, site2discs);

    // find the discontinutity surfaces
    FindDiscSurfaces(recons, query_cls, query_nc, pts, nosurf, surfaces, comps);

    // scale gradient when is too high
    //for (int i = 0; i < pts.size(); i++)
    //{
    //    float3 g = make_float3(pts[i].gradient[0], pts[i].gradient[1], pts[i].gradient[2]);
    //    if (length(g) > 60.0)
    //    {
    //        g = 60.0 * normalize(g);
    //        pts[i].gradient[0] = g.x;
    //        pts[i].gradient[1] = g.y;
    //        pts[i].gradient[2] = g.z;
    //    }
    //}
    
    // find the potential of each sample site with respect to all surfaces
    vector<vector<float> > sites_pot(pts.size());
    vector<vector<float> > sites_pgr(pts.size());
    for (int k = 0; k < pts.size(); k++)
    {
        sites_pot[k].resize(nosurf);
        //sites_pgr[k].resize(nosurf);
    }
    #pragma omp parallel for schedule(dynamic)
    for (int k = 0; k < pts.size(); k++)
    {
        int thn = omp_get_thread_num();
        int soff = thn * nosurf;
        Vector3f cpt = Vector3(pts[k].coordinate.x / recons->min_spc, pts[k].coordinate.y / recons->min_spc, pts[k].coordinate.z / recons->min_spc);
        float3 grad = make_float3(pts[k].gradient[0], pts[k].gradient[1], pts[k].gradient[2]);
        for (int i = 0; i < nosurf; i++)
        {
            // find the potential
            sites_pot[k][i] = FindPotential(surfaces[soff + i], cpt);
            /*if (abs(sites_pot[k][i]) < 1e6)
            {
                // directional gradient
                Vector3f ppt = cpt;
                Vector3f gpt;
                Expe::Color col;
                bool ret = surfaces[soff + i]->project(ppt, gpt, col);
                float3 gqx = make_float3(ppt.x - cpt.x, ppt.y - cpt.y, ppt.z - cpt.z);
                float3 gpx =  make_float3(gpt.x, gpt.y, gpt.z);

                //printf("%f %f,", length(gqx), abs(dot(normalize(gpx), normalize(gqx)))); 
                //if ( abs(dot(normalize(gpx), normalize(gqx))) < 0.7)
                //    printf("%f\n", abs(dot(normalize(gpx), normalize(gqx)))); 

                // check the gradient is not zero
                if (length(gqx) == 0.0)
                    gqx = gpx;
                if (length(gqx) != 0.0)
                    gqx = normalize(gqx);
                
                // now computed the directed gradient and its magnitude
                double g = dot(grad, gqx) * recons->min_spc;
                if (sites_pot[k][i] > 0.0)
                    g = -g;
                sites_pgr[k][i] = g;
            }*/
        }
    }

    // set discontinuity site as such when it is very closer to a point than any
    vector<bool> site_is_disc(pts.size());
    for (int i = 0; i < nosurf; i++)
    {
        for (int k = 0; k < comps[i].size(); k++)
        {
            int id = comps[i][k];
            int site = query_cls[id].id;
            if (site_is_disc[site] == true)
                continue;
            
            float3 c = recons->Space2Grid(pts[site].coordinate);
            c.x = myround(c.x);
            c.y = myround(c.y);
            c.z = myround(c.z);
            if ((c.x == 0) || (c.y == 0) || (c.z == 0) || 
                (c.x == recons->width() - 1) || (c.y == recons->height() - 1) || (c.z == recons->depth() - 1) ||
                (abs(sites_pot[site][i]) < 1e6))
            {
                site_is_disc[site] = true;
            }
        }
    }

    // also isolate those with big gradient
    /*double grad_limit = atof(parameters["GRAD_LIMIT"].c_str());
    for (int i = 0; i < pts.size(); i++)
    {
        float3 g = make_float3(pts[i].gradient[0], pts[i].gradient[1], pts[i].gradient[2]);
        if (length(g) > grad_limit)
        {
            site_is_disc[i] = true;
        }
    }*/

    // find the closest site to each point
    Tree* tree = NULL;
    FindClosest(recons, query_cls, query_nc, pts, site_is_disc, tree, nosurf, surfaces, site2discs);

    // find the natural coordinates
    FindNaturalCoordinates(recons, query_cls, query_nc, pts, nosurf, surfaces);
    
    // sibson interpolation
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < query_cls.size(); i++)
    {
        int3 c = recons->Addr2Coord(i);
        double msibv = SibsonInterpolation(recons, errm, query_cls, query_nc, pts, tree, nosurf, surfaces, i, sites_pot, sites_pgr);
        recons->Set(c.x, c.y, c.z, msibv);
        if ((i%1048576) == 0)
        {
            printf("."); fflush(stdout);
        }
    }
    printf("\n");

    // compute mse 
    double mse = 0.0;
    for (int i = 0; i < query_cls.size(); i++)
    {
        int3 c = recons->Addr2Coord(i);
        double v = origin->ProbeValueAt(c.x, c.y, c.z);
        mse += pow(v - recons->ProbeValueAt(c.x, c.y, c.z), 2.0);
    }
    mse /= query_cls.size();
    printf("MSE error is %e\n", mse);

    // free surface memory
    delete tree;
    for (int i = 0; i < surfaces.size(); i++)
    {
        // delete neighborhood
        BallNeighborhood* ballneighborhood = (BallNeighborhood*) (surfaces[i]->editNeighborhood());
        delete ballneighborhood;

        // delete weighting function
        Wf_OneMinusX2Power4* weightfunction = (Wf_OneMinusX2Power4*) (surfaces[i]->editWeightingFunction());
        delete weightfunction;

        // delete the points
        if (i < nosurf)
        {
            delete surfaces[i]->mpInputPoints;
        }
    }
    
    // write the output
    //recons->Write("sibtmp.nrrd");

    // time 
    timer.stop();
    cout << "Time for modified Sibson's step is " << (0.001 * timer.getElapsedTimeInMilliSec()) << " sec.\n\n";
}

void Refine(int iter, void* originc, vector<int>& nids, vector<float>& errm, vector<Sample_point>& pts, Tree*& tree, vector<closest_site>& query_cls)
{
    double lambda = atof(parameters["LAMBDA"].c_str());
    int nnews = atoi(parameters["NEWSAMPLES"].c_str());

    NrrdWrapper3D* origin = (NrrdWrapper3D*) originc;

    // find min and max. do scaling and log
    double mine = numeric_limits<double>::max();
    double maxe = numeric_limits<double>::min();
    for (int i = 0; i < errm.size(); i++)
    {
        mine = min(mine, double(errm[i]));
        maxe = max(maxe, double(errm[i]));
    }
    #pragma omp parallel for
    for (int i = 0; i < errm.size(); i++)
    {
        if (errm[i] == 0.0)
        {
            errm[i] = -1.0;
            continue;
        }
        errm[i] = (errm[i] - mine) / (maxe - mine);
        if (errm[i] <= 0.0)
        {
            errm[i] = -1.0;
            continue;
        }
        errm[i] = -log(errm[i]);
    }
    
    // now do the binning
    mine = numeric_limits<double>::max();
    maxe = numeric_limits<double>::min();
    for (int i = 0; i < errm.size(); i++)
    {
        if (errm[i] == -1.0)
            continue;
        mine = min(mine, double(errm[i]));
        maxe = max(maxe, double(errm[i]));
    }

    int nobins = 256;
    vector<vector<int> > bins(nobins);
    vector<double> bins_prob(nobins);
    for (int i = 0; i < errm.size(); i++)
    {
        if (errm[i] == -1.0)
            continue;
        int idx = myround(nobins * (errm[i] - mine) / (maxe - mine));
        idx = min(idx, nobins - 1);
        bins[idx].push_back(i);

        // compute the prob. for the point
        double p = 5.0 * (errm[i] - mine) / (maxe - mine);
        p = lambda * exp(-lambda * p);
        bins_prob[idx] += p;
        //bins_prob[idx] += 1.0 - (errm[i] - mine) / (maxe - mine);
    }

    // cdf computation
    double sum = 0.0;
    vector<double> cdf(nobins);
    for (int i = 0; i < nobins; i++)
    {
         sum += bins_prob[i];
         cdf[i] = sum;
    }
    for (int i = 0 ; i < nobins; ++i) 
    {
         cdf[i] /= sum;
         //printf("bin %d:\titems %d\tbin prob. %lf\tprob./cnt %lf\tcdf %lf\n", i, bins[i].size(), bins_prob[i], bins_prob[i]/bins[i].size(), cdf[i]);
    }

    // write the error to a file
    float* data = (float*) malloc(errm.size() * sizeof(float));
    memset(data, 0, errm.size() * sizeof(float));

    // select a set 
    vector<int> o2nc(pts.size());
    srand (time(NULL));
    #pragma omp parallel for
    for (int i = 0; i < nnews; i++)
    {
        bool failed = true;
        while (failed)
        {
            // find the bin
            double r = drand48();
            int bin = -1;
            for (int k = 0; k < nobins; k++)
            {
                if (((k == 0) || (cdf[k - 1] < r)) && (cdf[k] >= r))
                {
                    bin = k;
                    break;
                }
            }

            // select a random member of the bin
            int idx = lrand48() % bins[bin].size();
            idx = bins[bin][idx];

            // find closest and reduce probability based on it
            int closest = query_cls[idx].id;
            int cnt = o2nc[closest];
            double p = (cnt == 0) ? 1.0 : 1.0 / cnt;
            r = drand48();
            if (r > p)
                continue;

            // if alread in the list continue
            #pragma omp critical
            {
                if (data[idx] == 0.0)
                {
                    nids.push_back(idx);
                    data[idx] = 1.0;
                    o2nc[closest]++;
                    failed = false;
                }
            }
        }
    }

    // write the refinment information
    char iters[12];
    sprintf(iters, "%d", iter);
    string filename = parameters["OUTPUT_REFINE"] + string("_") + string(iters) + string(".nrrd");
    writeRawFile3D(data, filename.c_str(), make_int3(origin->width(),origin->height(),origin->depth()), make_double3(1,1,1));
    free(data);
}

void GenerateSurfaceMesh(void* reconsc, vector<NormalConstrainedSphericalMlsSurface*>& surfaces, int nosurf, vector<int> items)
{
    printf("Start extracting discontinuity mesh.\n");

    NrrdWrapper3D* recons = (NrrdWrapper3D*) reconsc;
    NrrdWrapper3D* data = new NrrdWrapper3D(teem_copy(recons->ni));
    memset(data->ni->data, 0, data->Size() * sizeof(float));

    // fill nrrd with the potential info
    #pragma omp parallel for
    for (int i = 0; i < data->Size(); i++)
    {
        int thn = omp_get_thread_num();
        int soff = thn * nosurf;
        float3 pt = data->Addr2Space(i);
        int3 gt = data->Addr2Coord(i);

        Vector3f cpt = Vector3f(pt.x / data->min_spc, pt.y / data->min_spc, pt.z / data->min_spc);
        double d = numeric_limits<double>::max();
        for (int k = 0; k < items.size(); k++)
        {
            int sid = items[k];
            d = min(d, FindPotential(surfaces[soff + sid], cpt)); 
        }
        ((float*)(data->ni->data))[i] = d;

        if (i%(256 * 1024) == 0)
        {
            printf("."); fflush(stdout);
        }
    }

    // compute the min and max potential
    double maxv = -numeric_limits<double>::max();
    double minv = numeric_limits<double>::max();
    for (int i = 0; i < data->Size(); i++)
    {
        int3 c = data->Addr2Coord(i);
        if (data->ProbeValueAt(c.x, c.y, c.z) < 1e+6)
        {
            maxv = max(maxv, data->ProbeValueAt(c.x, c.y, c.z));
            minv = min(minv, data->ProbeValueAt(c.x, c.y, c.z));
        }
    }
    printf("\nPotential min %lf max %lf\n", minv, maxv);
    data->Write("discon.nrrd");
    
    // convert nrrd to vtk and save it
    //const NrrdFormat _nrrdFormatVTK = {
    //    "VTK",
    //    AIR_FALSE,  /* isImage */
    //    AIR_TRUE,   /* readable */
    //    AIR_FALSE,  /* usesDIO */
    //    _nrrdFormatVTK_available,
    //    _nrrdFormatVTK_nameLooksLike,
    //    _nrrdFormatVTK_fitsInto,
    //    _nrrdFormatVTK_contentStartsLike,
    //    _nrrdFormatVTK_read,
    //    _nrrdFormatVTK_write
    //};
    NrrdIoState *nio = nrrdIoStateNew();
    nio->encoding = nrrdEncodingRaw;
    nio->format = nrrdFormatArray[4];
    nio->encoding = nrrdEncodingRaw;
    nrrdSave("kjagniqu.vtk", data->ni, nio);
    nio = nrrdIoStateNix(nio);

    // read the vtk
    vtkSmartPointer<vtkStructuredPointsReader> reader = vtkSmartPointer<vtkStructuredPointsReader>::New();
    reader->SetFileName("kjagniqu.vtk");
    reader->Update();

    // compute the isosurface
    // Create a 3D model using marching cubes
    vtkSmartPointer<vtkMarchingCubes> mc = vtkSmartPointer<vtkMarchingCubes>::New();
    mc->SetInputConnection(reader->GetOutputPort());
    mc->ComputeNormalsOn();
    mc->ComputeGradientsOn();
    mc->SetValue(0, 0.0);  // second value acts as threshold
    mc->Update();

    // get the mesh
    vtkPolyData* vtkMesh = mc->GetOutput();
    vtkPoints* points = vtkMesh->GetPoints();

    // new mesh points
    vtkPointData* pointsData = vtkMesh->GetPointData();
    vtkSmartPointer<vtkPolyData> vtkMeshNew = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> pointsNew = vtkSmartPointer<vtkPoints>::New();
    map<vtkIdType, vtkIdType> old2new;
    for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
    {
        double coord[3];
        points->GetPoint(i, coord);
        float3 gpt = data->Space2Grid(coord[0], coord[1], coord[2]);
        int3 c = make_int3(myround(gpt.x), myround(gpt.y), myround(gpt.z));
        bool throwit = false;
        for (int x = c.x - 1; x <= c.x + 1; x++)
        {
            for (int y = c.y - 1; y <= c.y + 1; y++)
            {
                for (int z = c.z - 1; z <= c.z + 1; z++)
                {
                    if (data->ValidGridPoint(x,y,z) == false)
                        continue;
                    if (data->ProbeValueAt(x,y,z) > 1e+6)
                    {
                        throwit = true;
                    }
                    if (throwit) break;
                }
                if (throwit) break;
            }
            if (throwit) break;
        }

        if (!throwit)
        {
            vtkIdType newid = pointsNew->InsertNextPoint(coord[0], coord[1], coord[2]);
            old2new.insert(pair<vtkIdType, vtkIdType>(i, newid));
        }
    }
    vtkMeshNew->SetPoints(pointsNew);
    printf("Original number of vertices is %d\n", points->GetNumberOfPoints());
    printf("Modified number of vertices is %d\n", vtkMeshNew->GetNumberOfPoints());
    delete data;

    // new mesh polys
    vtkSmartPointer<vtkCellArray> cellsNew = vtkSmartPointer<vtkCellArray>::New();
    vtkCellArray* vtkCells = vtkMesh->GetPolys();
    vtkCells->InitTraversal();
    vtkIdType npts;
    vtkIdType* pts;
    while (vtkCells->GetNextCell(npts, pts) != 0)
    {    
        bool skip = false;
        for (int j = 0; j < npts; j++)
        {
            if (old2new.find(pts[j]) == old2new.end())
            {
                skip = true;
                break;
            }
            
            pts[j] = old2new.find(pts[j])->second;
        }
        
        if (npts != 3) 
        {
            printf(".");
            continue;
        }
        
        if (skip == false)
            cellsNew->InsertNextCell(npts, pts);
    }
    vtkMeshNew->SetPolys(cellsNew);

    // write file
    vtkSmartPointer<vtkPolyDataWriter> vtkMeshWriter = vtkSmartPointer<vtkPolyDataWriter>::New();
    vtkMeshWriter->SetFileName("disc_surf.vtk");
    vtkMeshWriter->SetInputData(vtkMeshNew);
    vtkMeshWriter->Write();
}

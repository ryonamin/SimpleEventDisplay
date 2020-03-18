#include "Event.h"

// STL
#include <iostream>

// ROOT
#include "TRint.h"
#include "TGFrame.h"
#include "TGLabel.h"
#include "TGButton.h"
#include "TGButtonGroup.h"
#include "TGNumberEntry.h"
#include "TEvePad.h"
#include "TGLEmbeddedViewer.h"
#include "TEveViewer.h"
#include "TEveElement.h"

const float MAINFRAME_WIDTH  = 1000.;
const float MAINFRAME_HEIGHT = 800.;
const float CANVAS_WIDTH     = 1000.;
const float FRAME_L_WIDTH    = 300.;
const float FRAME_R_WIDTH    = 700.;
const float FRAME_R1_WIDTH   = 500.;
const float FRAME_R3_WIDTH   = 200.;
const float CANVAS_HEIGHT    = 760.;

int main(int argc, char** argv) {

  
  Event *ev = new Event();

  TRint* app = new TRint("App",&argc,argv);

  TGMainFrame* frm = new TGMainFrame(gClient->GetRoot());

  TEveManager::Create(kFALSE); // this create a global pointer, gEve. kFALSE: not map widow yet.
  // create eve canvas
  TEvePad* pad = new TEvePad();
  //pad->SetFillColor(kGray);
  pad->SetFillColor(kBlack);
  // create an embed GL viewer 
  TGLEmbeddedViewer* embviewer = new TGLEmbeddedViewer(frm, pad);
  frm->AddFrame(embviewer->GetFrame(), new TGLayoutHints(kLHintsNormal | kLHintsExpandX | kLHintsExpandY));
  // create a GL viewer 
  gEve->GetViewers()->DestroyElements(); // Remove default viewers (GetDefaultViewer will be the first children)
  TEveViewer* viewer = new TEveViewer("GLViewer");
  viewer->SetGLViewer(embviewer, embviewer->GetFrame());
  viewer->AddScene(gEve->GetGlobalScene());
  viewer->AddScene(gEve->GetEventScene());
  gEve->GetViewers()->AddElement(viewer);


  TGHorizontalFrame* bframeCTL = new TGHorizontalFrame(frm,CANVAS_WIDTH,(MAINFRAME_HEIGHT-CANVAS_HEIGHT)/2.,kFixedWidth);
  frm->AddFrame(bframeCTL,new TGLayoutHints(kLHintsNormal ));
  //  FrameCTL layout
  //  ----- ----------------------
  //  |   | |            |       |
  //  |   | |    R1      |  R3   |
  //  | L | |            |       |
  //  |   | ----------------------
  //  |   | |        R2          |
  //  ----- ----------------------

  TGHorizontalFrame* bframeL = new TGHorizontalFrame(bframeCTL,FRAME_L_WIDTH,(MAINFRAME_HEIGHT-CANVAS_HEIGHT)/2.,kFixedWidth);
  bframeL->SetLayoutManager(new TGMatrixLayout(bframeL,8,2)); // row, column
  bframeCTL->AddFrame(bframeL);
  TGVerticalFrame* bframeR = new TGVerticalFrame(bframeCTL,FRAME_R_WIDTH,(MAINFRAME_HEIGHT-CANVAS_HEIGHT)/2.,kFixedWidth);
  bframeCTL->AddFrame(bframeR);

  TGHorizontalFrame* bframeR13 = new TGHorizontalFrame(bframeR,FRAME_R_WIDTH,(MAINFRAME_HEIGHT-CANVAS_HEIGHT)/2.,kFixedWidth);
  bframeR->AddFrame(bframeR13);

  TGCompositeFrame* bframeR1 = new TGCompositeFrame(bframeR13,FRAME_R_WIDTH,(MAINFRAME_HEIGHT-CANVAS_HEIGHT)/2.,kFixedWidth);
  bframeR1->SetLayoutManager(new TGMatrixLayout(bframeR1,7,2)); // row, column
  bframeR13->AddFrame(bframeR1, new TGLayoutHints(kLHintsCenterX));

  TGHorizontalFrame* bframeR2 = new TGHorizontalFrame(bframeR,FRAME_R1_WIDTH,(MAINFRAME_HEIGHT-CANVAS_HEIGHT)/2.,kFixedWidth);
  bframeR2->SetLayoutManager(new TGMatrixLayout(bframeR2,2,8)); // row, column
  //bframeR->AddFrame(bframeR2, new TGLayoutHints( kLHintsBottom ));
  bframeR->AddFrame(bframeR2);

  TGVerticalFrame* bframeR3 = new TGVerticalFrame(bframeR13,FRAME_R3_WIDTH,(MAINFRAME_HEIGHT-CANVAS_HEIGHT)/2.,kFixedWidth);
  bframeR13->AddFrame(bframeR3);

  // Labels used in frames
  TGLabel* label_col       = new TGLabel(bframeR1," Collection : ");
  TGLabel* label_vis       = new TGLabel(bframeR1," Visibility : ");
  TGLabel* label_mcpfo     = new TGLabel(bframeR1," MC-PFO Link collection: ");
  TGLabel* label_bfield    = new TGLabel(bframeL," B field [T]: ");
  TGLabel* label_rmax      = new TGLabel(bframeL," R max    [mm]: ");
  TGLabel* label_zmax      = new TGLabel(bframeL," Z max    [mm]: ");
  TGLabel* label_empty_L   = new TGLabel(bframeL," ");
  TGLabel* label_empty_R1  = new TGLabel(bframeR1," ");
  TGLabel* label_empty_R2  = new TGLabel(bframeR2," ");

  // Frame L
  // Row 1 Column 1 
  TGTextButton* b_open = new TGTextButton(bframeL,"Open File");
  b_open->Connect("Clicked()","Event",ev,"OpenFile()");
  bframeL->AddFrame(b_open,new TGLayoutHints(kLHintsLeft));
  b_open->Resize(80,25);
  // Row 1 Column 2 
  bframeL->AddFrame(label_empty_L,new TGLayoutHints(kLHintsLeft));
  // Row 2 Column 1 
  bframeL->AddFrame(label_empty_L,new TGLayoutHints(kLHintsLeft));
  // Row 2 Column 2 
  bframeL->AddFrame(label_empty_L,new TGLayoutHints(kLHintsLeft));
  // Row 3 Column 1 // Axis visibility
  TGCheckButton *c_axis_vis = new TGCheckButton(bframeL, "Axis (1 mm)",6);
  c_axis_vis->SetState(kButtonDown);
  ev->SetAxisVisState(c_axis_vis);
  bframeL->AddFrame(c_axis_vis,new TGLayoutHints(kLHintsLeft));
  // Row 3 Column 2 
  bframeL->AddFrame(label_empty_L,new TGLayoutHints(kLHintsLeft));
  // Row 4 Column 1 // B field
  bframeL->AddFrame(label_bfield, new TGLayoutHints(kLHintsLeft));
  // Row 4 Column 2 
  TGNumberEntry* ne_bfield = new TGNumberEntry(bframeL);
  ne_bfield->Connect("ValueSet(Double_t)","Event",ev,"UpdatePropagator()");
  bframeL->AddFrame(ne_bfield,new TGLayoutHints(kLHintsLeft));
  ev->SetBField(ne_bfield->GetNumberEntry());
  // Row 5 Column 1 
  bframeL->AddFrame(label_rmax, new TGLayoutHints(kLHintsLeft));
  // Row 5 Column 2 
  TGNumberEntry* ne_rmaxfield = new TGNumberEntry(bframeL);
  ne_rmaxfield->Connect("ValueSet(Double_t)","Event",ev,"UpdatePropagator()");
  bframeL->AddFrame(ne_rmaxfield,new TGLayoutHints(kLHintsLeft));
  ev->SetRmaxField(ne_rmaxfield->GetNumberEntry());
  // Row 6 Column 1 
  bframeL->AddFrame(label_zmax, new TGLayoutHints(kLHintsLeft));
  // Row 6 Column 2 
  TGNumberEntry* ne_zmaxfield = new TGNumberEntry(bframeL);
  ne_zmaxfield->Connect("ValueSet(Double_t)","Event",ev,"UpdatePropagator()");
  bframeL->AddFrame(ne_zmaxfield,new TGLayoutHints(kLHintsLeft));
  ev->SetZmaxField(ne_zmaxfield->GetNumberEntry());
  // Row 7 Column 1 
  bframeL->AddFrame(label_empty_L,new TGLayoutHints(kLHintsLeft));
  // Row 7 Column 2 
  bframeL->AddFrame(label_empty_L,new TGLayoutHints(kLHintsLeft));
  // Row 8 Column 1 
  TGTextButton* b_exit = new TGTextButton(bframeL,"Exit","gApplication->Terminate(0)");
  bframeL->AddFrame(b_exit,new TGLayoutHints(kLHintsLeft | kLHintsExpandY));
  b_exit->Resize(80,25);
  // Row 8 Column 2 
  bframeL->AddFrame(label_empty_L,new TGLayoutHints(kLHintsLeft));






  // Frame R1 
  // Row 1 Column 1
  bframeR1->AddFrame(label_vis, new TGLayoutHints(kLHintsLeft));
  // Row 1 Column 2
  bframeR1->AddFrame(label_col, new TGLayoutHints(kLHintsLeft));
  // Row 2 Column 1 // MCParticle
  TGCheckButton *c_mcps_vis = new TGCheckButton(bframeR1, "MCParticle",1);
  c_mcps_vis->SetState(kButtonDown);
  bframeR1->AddFrame(c_mcps_vis,new TGLayoutHints(kLHintsLeft));
  ev->SetMCParticleVisState(c_mcps_vis);
  // Row 2 Column 2
  TGComboBox* list_mcps_col = new TGComboBox(bframeR1,1); // ComboBox ID = 1
  bframeR1->AddFrame(list_mcps_col,new TGLayoutHints(kLHintsLeft));
  list_mcps_col->Resize(200,20);
  ev->SetMCParticleColList(list_mcps_col);
  // Row 3 Column 1 // Tracks
  TGCheckButton *c_tracks_vis = new TGCheckButton(bframeR1, "Track",2);
  c_tracks_vis->SetState(kButtonDown);
  bframeR1->AddFrame(c_tracks_vis,new TGLayoutHints(kLHintsLeft));
  ev->SetTrackVisState(c_tracks_vis);
  // Row 3 Column 2
  TGComboBox* list_tracks_col = new TGComboBox(bframeR1,2); // ComboBox ID = 2
  bframeR1->AddFrame(list_tracks_col,new TGLayoutHints(kLHintsLeft));
  list_tracks_col->Resize(200,20);
  ev->SetTrackColList(list_tracks_col);
  // Row 4 Column 1 // Primary Vertex
  TGCheckButton *c_pvtxtracks_vis = new TGCheckButton(bframeR1, "Primary Vertex ",3);
  c_pvtxtracks_vis->SetState(kButtonUp); //c_tracks_vis->SetState(kButtonDown);
  bframeR1->AddFrame(c_pvtxtracks_vis,new TGLayoutHints(kLHintsLeft));
  ev->SetPVtxTrackVisState(c_pvtxtracks_vis);
  // Row 4 Column 2
  TGComboBox* list_pvtxtracks_col = new TGComboBox(bframeR1,3); // ComboBox ID = 3
  bframeR1->AddFrame(list_pvtxtracks_col,new TGLayoutHints(kLHintsLeft));
  list_pvtxtracks_col->Resize(200,20);
  ev->SetPVtxTrackColList(list_pvtxtracks_col);
  // Row 5 Column 1 // Secondary Vertex
  TGCheckButton *c_svtxtracks_vis = new TGCheckButton(bframeR1, "Secondary Vertex",4);
  c_svtxtracks_vis->SetState(kButtonUp); //c_tracks_vis->SetState(kButtonDown);
  bframeR1->AddFrame(c_svtxtracks_vis,new TGLayoutHints(kLHintsLeft));
  ev->SetSVtxTrackVisState(c_svtxtracks_vis);
  // Row 5 Column 2
  TGComboBox* list_svtxtracks_col = new TGComboBox(bframeR1,4); // ComboBox ID = 4
  bframeR1->AddFrame(list_svtxtracks_col,new TGLayoutHints(kLHintsLeft));
  list_svtxtracks_col->Resize(200,20);
  ev->SetSVtxTrackColList(list_svtxtracks_col);
  // Row 6 Column 1 // Jet
  TGCheckButton *c_jets_vis = new TGCheckButton(bframeR1, "Jet  ",5);
  c_jets_vis->SetState(kButtonUp);
  bframeR1->AddFrame(c_jets_vis,new TGLayoutHints(kLHintsLeft));
  ev->SetJetVisState(c_jets_vis);
  // Row 6 Column 2
  TGComboBox* list_jets_col = new TGComboBox(bframeR1,5); // ComboBox ID = 5
  bframeR1->AddFrame(list_jets_col,new TGLayoutHints(kLHintsLeft));
  list_jets_col->Resize(200,20);
  ev->SetJetColList(list_jets_col);
  // Row 7 Column 1
  bframeR1->AddFrame(label_mcpfo, new TGLayoutHints(kLHintsCenterX));
  // Row 7 Column 2
  TGComboBox* list_rel_col = new TGComboBox(bframeR1,6); // ComboBox ID = 6
  bframeR1->AddFrame(list_rel_col,new TGLayoutHints(kLHintsLeft));
  list_rel_col->Resize(200,20);
  ev->SetRelColList(list_rel_col);

  // Frame R2
  // Row1 Column1
  bframeR2->AddFrame(label_empty_R2,new TGLayoutHints(kLHintsLeft));
  bframeR2->AddFrame(label_empty_R2,new TGLayoutHints(kLHintsLeft));
  bframeR2->AddFrame(label_empty_R2,new TGLayoutHints(kLHintsLeft));
  bframeR2->AddFrame(label_empty_R2,new TGLayoutHints(kLHintsLeft));
  TGLabel* label_ev = new TGLabel(bframeR2,"Event number : ");
  label_ev->Resize(90,25);
  bframeR2->AddFrame(label_ev, new TGLayoutHints(kLHintsCenterX));
  bframeR2->AddFrame(label_empty_R2,new TGLayoutHints(kLHintsLeft));
  bframeR2->AddFrame(label_empty_R2,new TGLayoutHints(kLHintsLeft));
  bframeR2->AddFrame(label_empty_R2,new TGLayoutHints(kLHintsLeft));
  // Row2 Column1
  bframeR2->AddFrame(label_empty_R2,new TGLayoutHints(kLHintsLeft));
  // Row2 Column2
  bframeR2->AddFrame(label_empty_R2,new TGLayoutHints(kLHintsLeft));
  // Row2 Column3
  bframeR2->AddFrame(label_empty_R2,new TGLayoutHints(kLHintsLeft));
  // Row2 Column4
  //TGLabel* label_ev = new TGLabel(bframeR2,"Event number : ");
  //bframeR2->AddFrame(label_ev, new TGLayoutHints(kLHintsCenterX));
  bframeR2->AddFrame(label_empty_R2,new TGLayoutHints(kLHintsLeft));
  //label_ev->Resize(90,25);
  // Row2 Column5
  TGTextButton* b_prev = new TGTextButton(bframeR2,"Prev");
  b_prev->Connect("Clicked()","Event",ev,"Prev()");
  bframeR2->AddFrame(b_prev,new TGLayoutHints(kLHintsCenterX));
  b_prev->Resize(50,20);
  // Row2 Column6
  TGNumberEntry* ne = new TGNumberEntry(bframeR2);
  ne->Connect("ValueSet(Long_t)","Event",ev,"Goto()");
  bframeR2->AddFrame(ne,new TGLayoutHints(kLHintsCenterX));
  ev->SetNevtField(ne->GetNumberEntry());
  // Row2 Column7
  TGTextButton* b_reload = new TGTextButton(bframeR2,"Reload");
  b_reload->Connect("Clicked()","Event",ev,"Update()");
  bframeR2->AddFrame(b_reload,new TGLayoutHints(kLHintsCenterX));
  b_reload->Resize(50,20);
  // Row2 Column8
  TGTextButton* b_next = new TGTextButton(bframeR2,"Next");
  b_next->Connect("Clicked()","Event",ev,"Next()");
  bframeR2->AddFrame(b_next,new TGLayoutHints(kLHintsCenterX));
  b_next->Resize(50,20);


  // frame R3
  TGButtonGroup* rb_views = new TGButtonGroup(bframeR3,"View",kHorizontalFrame);
  TGRadioButton* rb_xy = new TGRadioButton(rb_views, new TGHotString("&XY"));
  TGRadioButton* rb_yz = new TGRadioButton(rb_views, new TGHotString("&YZ"));
  rb_xy->SetState(kButtonDown);
  rb_views->Show();
  rb_views->Connect("Clicked(Int_t)","Event",ev,"ViewSet(Int_t)");
  bframeR3->AddFrame(rb_views,new TGLayoutHints(kLHintsLeft));
  // Dump MCParticles
  TGTextButton* b_mcdump = new TGTextButton(bframeR3,"Dump MCParticles");
  b_mcdump->Connect("Clicked()","Event",ev,"DumpMCParticles()");
  bframeR3->AddFrame(b_mcdump,new TGLayoutHints(kLHintsCenterX));
  b_mcdump->Resize(50,20);




  frm->MapSubwindows();
  frm->Resize(MAINFRAME_WIDTH,MAINFRAME_HEIGHT);
  frm->MapWindow();

  app->Run(kTRUE);
  return 0;
}
